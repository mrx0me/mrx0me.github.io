---
layout: post
title: Path Traversal
image: "/assets/images/web_attacks/path_traversal.png"
category: Web Exploitation
author: mr0me
---

## Introduction

Imaginez pouvoir lire n'importe quel fichier d'une application web ou d'un programme informatique. Ce scénario, aussi inquiétant soit-il, devient réalité lorsque le programme présente une faille de sécurité appelée **Path Traversal** (ou Directory Traversal).

Une attaque de type « directory traversal », « path traversal » ou « dot-dot-slash » exploite une validation ou une désinfection insuffisante des noms de fichiers fournis par l'utilisateur. Des caractères représentant « accès au répertoire parent » (comme `../`) sont alors transmis à l'API du système de fichiers, permettant ainsi d'accéder à des fichiers normalement inaccessibles.

*(Source : Wikipédia)*

![Illustration - Source : www.fastly.com](https://www.fastly.com/cimages/ocb1q9kflo7k/RwO9SUbnSDBpBNKskZFHd/11e8d23be38d16ca7c0cb945edc879cb/traveral_with_caption.png?auto=avif&crop=1265:735,smart&width=900)

## Exemple de code vulnérable

```python
from flask import Flask, request, send_file
import os

app = Flask(__name__)

@app.route('/download')
def download_file():
    # Récupère le nom du fichier depuis les paramètres de la requête
    filename = request.args.get('filename')
    
    # Dossier de base où se trouvent les fichiers
    base_dir = '/var/www/uploads/'
    
    # Construction directe du chemin (VULNÉRABLE)
    file_path = os.path.join(base_dir, filename)
    
    # Envoie le fichier sans vérification
    return send_file(file_path)

if __name__ == '__main__':
    app.run()
```

### Analyse de la vulnérabilité

Dans ce code, la faille se trouve à la ligne `file_path = os.path.join(base_dir, filename)`. Cette instruction récupère directement l'entrée utilisateur et la joint au chemin du répertoire local, avant de retourner le fichier correspondant dans l'arborescence du système de fichiers.

**Fonctionnement normal :**  
L'auteur de ce code s'attend à recevoir des requêtes comme :
```url
http://exemple.com/download?filename=image.png
```
Avec `base_dir = '/var/www/uploads/'` et `filename = 'image.png'`, le fichier retourné sera `/var/www/uploads/image.png`. Jusque là, rien de malveillant.

**Comportement malveillant :**  
Mais comme aucun filtre n'est mis en place, un attaquant peut remonter dans l'arborescence pour lire le fichier de son choix. Par exemple, si `filename = '../../etc/passwd'`, le chemin construit devient `/var/www/uploads/../../etc/passwd`, ce qui est normalisé en `/etc/passwd`. Ce fichier contient les comptes utilisateurs d'un système Linux et est considéré comme sensible.

Cette attaque représente l'exploitation la plus élémentaire de cette faille.

## Tentatives de protection inefficaces

Parfois, les développeurs tentent de mettre en place certains filtres, mais ceux-ci s'avèrent souvent insuffisants.

### 1. Suppression des caractères `../`

**Tentative :** Supprimer les séquences `../` pour prévenir le problème.

```python
filename = request.args.get('filename').replace('../', '')
```

**Contournement :** Ce filtre est facile à contourner en utilisant `....//....//etc/passwd`. Après suppression des `../`, il restera `../../etc/passwd` car la suppression n'est appliquée qu'une seule fois.

**Variantes de contournement :**
- `..././..././etc/passwd` (les `./` peuvent être supprimés différemment)
- Encodage URL : `%2e%2e%2f` pour `../`
- Encodage double : `%252e%252e%252f`
- Sur Windows : `..\` ou `..\\`

### 2. Ajout forcé d'une extension

**Tentative :** Le développeur ajoute automatiquement une extension au nom du fichier côté serveur.

```python
file_path = os.path.join(base_dir, filename + '.pdf')
```

**Contournement :** Cette protection peut être contournée en utilisant le caractère nul (`%00`) qui marque la fin d'une chaîne en C et dans certains systèmes.

```
http://example.com/download?filename=../../etc/passwd%00
```

Le chemin devient `/var/www/uploads/../../etc/passwd%00.pdf`, mais le caractère nul est interprété comme une fin de chaîne, donnant accès à `/etc/passwd`.

### 3. Validation sur liste blanche (bonne pratique à éviter mal implémentée)

```python
if '..' in filename or '/' in filename or '\\' in filename:
    abort(403)
```

**Problème :** Cette approche ignore les encodages URL, les caractères Unicode, ou les chemins absolus commençant par `/`.

## Pourquoi cette vulnérabilité est-elle si répandue ?

Le problème n'est pas lié à un système d'exploitation spécifique, ni à un framework, langage de programmation ou bibliothèque particuliers. C'est avant tout une **erreur de conception** : l'application fait confiance aux données fournies par l'utilisateur pour construire des chemins d'accès au système de fichiers.

La vulnérabilité peut se manifester dans de nombreux contextes :
- Téléchargement de fichiers
- Lecture de logs ou de configurations
- Génération de rapports

## Mesures de remédiation

La meilleure approche pour contrer ce type de problème est de **ne jamais faire confiance aux entrées utilisateur** (comme pour beaucoup d'autres failles de sécurité). Voici les solutions recommandées :

### Solution 1 : Normalisation et validation stricte (Recommandée)

```python
from flask import Flask, request, send_file, abort
import os

app = Flask(__name__)

@app.route('/download')
def download_file():
    filename = request.args.get('filename')
    base_dir = os.path.realpath('/var/www/uploads/')
    
    # Nettoyer et normaliser le chemin complet
    requested_path = os.path.realpath(os.path.join(base_dir, filename))
    
    # Vérifier que le chemin résultant est bien dans le dossier autorisé
    if not requested_path.startswith(base_dir):
        abort(403)  # Accès refusé
    
    # Vérifier que le fichier existe et n'est pas un répertoire
    if not os.path.isfile(requested_path):
        abort(404)
    
    return send_file(requested_path)
```

### Solution 2 : Liste blanche des fichiers autorisés 

```python
ALLOWED_FILES = {
    'rapport.pdf': '/var/www/uploads/rapport.pdf',
    'image.png': '/var/www/uploads/image.png',
    'data.csv': '/var/www/uploads/data.csv'
}

@app.route('/download')
def download_file():
    filename = request.args.get('filename')
        if filename not in ALLOWED_FILES:
        abort(404)
    return send_file(ALLOWED_FILES[filename])
```

### Solution 3 : Utiliser un identifiant numérique

```python
# Mapping ID -> nom de fichier réel
FILE_MAPPING = {
    1: 'rapport.pdf',
    2: 'image.png',
    3: 'document.txt'
}

@app.route('/download/<int:file_id>')
def download_file(file_id):
    if file_id not in FILE_MAPPING:
        abort(404)
    filename = FILE_MAPPING[file_id]
    return send_file(f'/var/www/uploads/{filename}')
```

## Ce qu'il ne faut **JAMAIS** faire

```python
# Simple suppression des ".." (contournable)
filename = request.args.get('filename').replace('..', '')

# Vérification incomplète (ne protège pas contre ..\ sous Windows)
if '..' not in filename:
    send_file(f'/uploads/{filename}')

# Blacklist insuffisante (contournable avec encodage URL)
forbidden = ['..', '/', '\\']
if all(c not in filename for c in forbidden):
    send_file(...)

# Confiance dans l'extension ajoutée automatiquement
file_path = os.path.join(base_dir, filename + '.pdf')  # Vulnérable au null byte
```

## Ressources pour approfondir

### Pratique et apprentissage
- **[OWASP Directory Traversal](https://owasp.org/www-project-web-security-testing-guide/v42/4-Web_Application_Security_Testing/05-Authorization_Testing/01-Testing_Directory_Traversal_File_Include)** - Guide complet de test
- **[PortSwigger Web Security Academy](https://portswigger.net/web-security/file-path-traversal)** - Laboratoires interactifs gratuits pour pratiquer

### Labs et challenges
- **TryHackMe** - Room "File Inclusion"
- **HackTheBox** - Machines avec path traversal
- **Root-Me** - Challenge "Directory traversal"

## Conclusion

Le Path Traversal est une vulnérabilité classique mais toujours dangereuse. Sa prévention repose sur un principe simple : **ne jamais construire de chemins d'accès à partir d'entrées utilisateur non validées**. En appliquant les mesures de remédiation présentées dans cet article, vous protégerez efficacement vos applications contre ce type d'attaque.
