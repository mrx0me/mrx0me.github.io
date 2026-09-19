---
layout: post
title: Format String Vulnerability
image: "https://cdn.prod.website-files.com/6a020fca21245d64af2c19d8/6a020fca21245d64af2c33bd_Format%20String%20Attacks%20preview.png"
category: "bin exploitation"
author: mr0me
---

# Les failles de format de chaîne de caractères

## Introduction

Une faille de format de chaîne de caractères permet de faire un dump de la mémoire d'un programme simplement à cause d'une mauvaise utilisation d'une fonction de formatage. C'est une vulnérabilité critique et souvent sous-estimée.

Pouvez-vous deviner dans ce code le bug qui s'y trouve ?

## Le code vulnérable

```C
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    if (argc < 2){
        printf("Veuillez entrer un argument\n");
        exit(1);
    }

    char secret[] = "My secret is here";
    printf(argv[1]);
    return 0;
}
```

Le bug se situe au niveau de la fonction `printf`. Normalement, nous devons lui passer en premier argument le format, c'est-à-dire la directive indiquant comment afficher notre argument. Ici, l'argument fourni par l'utilisateur (`argv[1]`) est utilisé directement comme chaîne de format, ce qui permet à l'utilisateur d'injecter des spécificateurs de format et de lire arbitrairement la mémoire du programme.

Mon but n'est pas de parler du langage C en particulier, mais de cette catégorie de failles de format de chaîne de caractères qui peut aussi être exploitée en Python. Mais puisque pour notre premier exemple nous disposons d'un code vulnérable en C, nous devons rappeler le format et le type de données qui est affiché lorsque nous les utilisons :

| Spécificateur | Type affiché |
|---------------|--------------|
| `%d`  | décimal signé |
| `%ld` | décimal signé (long) |
| `%u`  | décimal non signé |
| `%lu` | décimal non signé (long) |
| `%o`  | octal non signé |
| `%lo` | octal non signé (long) |
| `%x`  | hexadécimal non signé |
| `%lx` | hexadécimal non signé (long) |
| `%f`  | décimal virgule fixe |
| `%lf` | décimal virgule fixe (double) |
| `%e`  | décimal notation exponentielle |
| `%le` | décimal notation exponentielle (double) |
| `%g`  | décimal, représentation la plus courte parmi `%f` et `%e` |
| `%lg` | décimal, représentation la plus courte parmi `%lf` et `%le` |
| `%c`  | caractère |
| `%s`  | chaîne de caractères |
| `%n`  | écrit le nombre de caractères déjà affichés dans un pointeur (arme d'écriture) |

Chaque `%lx` va lire 8 octets sur la pile (sur architecture 64 bits) et les afficher en hexadécimal. En multipliant les `%lx`, on remonte la pile.

## Compilation et première exécution

Compilons notre programme avec `gcc` :

```bash
gcc -o vulnerable vulnerable.c
./vulnerable "mon argument"
```

![Compilation and execution](/assets/images/formant_string/compilation_gcc.png)

Rien d'anormal. Le programme affiche simplement `mon argument`. Mais maintenant, essayons :

```bash
./vulnerable "%lx"
```

Le programme affiche une valeur hexadécimale au lieu de la chaîne `%lx`. La faille est confirmée.

## Extraction hexadécimale de la mémoire

Pour lire plus loin, il faut multiplier les `%lx`. On génère la charge utile avec Python :

```python
payload = "%lx" * 50
print(payload)
```

Puis on passe cette chaîne au programme :

```bash
./vulnerable $(python3 -c "print('%lx'*50)")
```

![extraction hex](/assets/images/formant_string/hex_extract.png)

On obtient une longue suite de valeurs hexadécimales. Pour donner du sens à ce bruit, il faut convertir chaque paire d'octets en caractère lisible :

```python
data = input('')
out = ''

for i in range(0, len(data), 2):
    ops = data[i:i+2]
    out = chr(int(ops, base=16)) + out

print(out)
```

On redirige la sortie du programme vulnérable vers ce script :

```bash
./vulnerable $(python3 -c "print('%lx'*100)") | python3 convert.py
```

![piped to script](/assets/images/formant_string/out_pipe_script.png)

Des caractères étranges apparaissent. Certains sont des adresses mémoire, d'autres des valeurs aléatoires. Mais parmi eux, des fragments lisibles émergent.

## Extraction des variables d'environnement

Augmentons encore le nombre de `%lx` :

![more extract data](/assets/images/formant_string/more%20extract%20data.png)

Et soudain, des chaînes familières apparaissent :

```text
/home:/snap/bal/games/usr/locr/games:/bin:/usn:/sbin::/usr/biusr/sbinal/bin://usr/local/sbin:/usr/loccal/bin:/oem/.loTH=/
...
Mrx00mGER=locaION_MANAcadSESSgedabagaxcxdxbxeLORS=GxfzshLSCOusr/bin/SHELL=/ is herey secret%lx%lxmlx%lx%lxx%lx%lx%...
...
```

On distingue le `PATH` de la machine, des variables d'environnement, et même un fragment de `"My secret is here"`, la variable `secret` déclarée dans le programme et jamais utilisée. Elle était pourtant là, sur la pile, attendant d'être lue.

Parfois, il faut réexécuter plusieurs fois pour obtenir un résultat stable. La pile varie selon l'environnement, les arguments, et l'ASLR.

## Le même bug en Python

Python, langage moderne et sécurisé, n'est pas à l'abri. Considérez ce code :

```python
user_input = input("Entrez votre message : ")
print(user_input.format())
```

Si l'utilisateur entre `{user.__class__.__mro__[1].__subclasses__()}`, Python va tenter d'évaluer cette expression. Selon le contexte, cela peut mener à une fuite d'information ou même à une exécution de code arbitraire.

Voici un exemple plus concret et plus dangereux :

```python
import os

class User:
    def __init__(self, name):
        self.name = name

user = User("Alice")
template = input("Format : ")
print(template.format(user=user))
```

Si l'utilisateur entre :

```
{user.__class__.__init__.__globals__[os].environ}
```

Il obtient l'ensemble des variables d'environnement du processus. Et avec un peu d'ingéniosité, il peut remonter jusqu'à `os.system` et exécuter des commandes.

![](/assets/images/formant_string/python_exploit.png)

C'est l'équivalent Python du `%lx` en C : une sonde qui explore l'espace des objets accessibles.


## Preuves réelles : les CVE existent

Les failles de format de chaîne ne sont pas une menace théorique. Du simple éditeur de texte aux plateformes de sécurité, du C au Python, ces vulnérabilités ont été officiellement assignées à de nombreux CVE, impliquant fuite d'information, déni de service, voire exécution de code à distance.

### Notepad++ (CVE-2026-3008)

Notepad++ 8.9.3 et versions antérieures souffrent d'une injection de format de chaîne. Dans le fichier `nativeLang.xml`, l'attribut `find-result-hits` est transmis sans validation à `wsprintfW` comme chaîne de format. Un attaquant peut remplacer le pack de langue par un fichier malveillant (contenant `%s`, `%08lx`, etc.) et provoquer une fuite de la pile et des registres, ainsi qu'un crash de l'application. Score CVSS : 6.6.

### GNU nano (CVE-2026-6390)

GNU nano présente un défaut dans la gestion des erreurs multi-fichiers. Lorsqu'un utilisateur ouvre plusieurs fichiers au démarrage et que l'un déclenche une erreur de niveau ALERT, un nom de fichier spécialement conçu (contenant des spécificateurs `printf`) est réinterprété. La vulnérabilité peut entraîner une fuite de la pile, un déni de service (crash), voire une écriture mémoire arbitraire potentielle. Affecte Debian bullseye à trixie, ainsi que Red Hat Enterprise Linux 6 à 10.

### Routeur Actiontec (CVE-2024-6145)

Le serveur HTTP du routeur Actiontec WCB6200Q souffre d'une faille de format de chaîne. Un attaquant peut déclencher les spécificateurs de format dans une chaîne contrôlée par l'utilisateur via un en-tête Cookie malveillant, **sans authentification**, et exécuter du code arbitraire dans le contexte du serveur HTTP. Score CVSS : 8.8 (élevé).

### Zabbix (CVE-2024-42330)

L'objet `HttpRequest` de Zabbix présente un défaut de format de chaîne permettant à un utilisateur authentifié de fuiter des chaînes internes et d'accéder à des attributs cachés via une requête HTTP spécialement conçue. Score CVSS : **9.1 (critique)**.

### L'écosystème Python également touché

**asteval (CVE-2025-24359)** : la bibliothèque `asteval` gère incorrectement les nœuds AST `FormattedValue`. Un attaquant peut contrôler la chaîne de format pour s'échapper du bac à sable et exécuter du code Python arbitraire. Le PoC utilise `f"{dict.mro()[1]:'\\x7B__fstring__.__getattribute__.s\\x7D'}"` pour contourner les restrictions et appeler `os.system("whoami")`.

**AccessControl (PYSEC-2026-2325)** : la fonctionnalité `format` de Python permet de lire récursivement les attributs d'objets accessibles. `str.format_map` reste dangereux et peut mener à une fuite d'information grave. Affecte tous les scénarios où un utilisateur non fiable peut créer et exécuter du code Python contrôlé.

### Autres exemples notables

- **CVE-2026-54268** : la fonction `formatDate` du framework Angular souffre d'un déni de service par format de chaîne. CVSS : 8.2.
- **CVE-2026-6242** : le service ONVIF de la caméra Tapo C520WS présente une faille de format de chaîne. CVSS : 6.8.

Ces CVE démontrent que les failles de format de chaîne constituent une catégorie de menace persistante, cross-langage et cross-plateforme. Que ce soit le `printf` du C ou le `str.format` de Python, dès que la chaîne de format est contrôlée par l'utilisateur, le risque est bien réel. 



## Les mesures de protection

**En C :**
- Toujours utiliser `printf("%s", argv[1])` au lieu de `printf(argv[1])`.
- Activer les protections du compilateur : `-Wformat-security`, `-D_FORTIFY_SOURCE=2`.
- Utiliser des fonctions sûres comme `fputs` ou `puts` quand c'est possible.

**En Python :**
- Ne jamais formater une chaîne contrôlée par l'utilisateur.
- Valider et échapper les entrées.
- Utiliser des bibliothèques de templating sécurisées (Jinja2 avec sandbox, par exemple).

**Au niveau système :**
- Activer l'ASLR (Address Space Layout Randomization).
- Utiliser des compilateurs avec PIE (Position Independent Executable).
- Surveiller les entrées utilisateur dans les journaux.

## Conclusion

Les failles de format de chaîne de caractères sont parmi les plus anciennes et les plus dangereuses. Elles permettent non seulement de lire la mémoire, mais aussi d'écrire arbitrairement via `%n`, ce qui peut mener à une exécution de code. La meilleure défense reste de ne jamais passer une chaîne contrôlée par l'utilisateur directement à une fonction de formatage, et de toujours spécifier explicitement le format attendu.

**À retenir :** une chaîne de format contrôlée par l'utilisateur est une porte ouverte sur la mémoire. En C comme en Python, la règle est simple : ne faites jamais confiance à l'entrée utilisateur pour définir un format.