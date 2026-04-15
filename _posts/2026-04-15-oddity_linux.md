---
layout: post
title: Les bizzaries de Linux :) de certain fichiers Linux
image: "https://upload.wikimedia.org/wikipedia/en/thumb/d/de/Ccpenguin%2C_the_ancestor_of_Tux.jpg/250px-Ccpenguin%2C_the_ancestor_of_Tux.jpg"
category: Linux
author: mr0me
---

Linux, c’est souvent déroutant, surtout quand on s’intéresse à certains fichiers qu’on y trouve mais qui conservent une utilité insoupçonnée.
Et pour en parler dans cet article, nous allons découvrir quelques-uns de ces fichiers mystiques.

Premièrement, dans la philosophie des systèmes Unix (dont Linux est un descendant), tout est fichier. Cela signifie que répertoires, exécutables, processus, liens, mais aussi les périphériques matériels sont représentés et manipulables via des fichiers spéciaux situés dans le répertoire /dev.

Parmi ces fichiers qui, au départ, peuvent nous intriguer, on trouve les suivants.

## Je suis nul

`/dev/null`

C’est sans doute le plus utilisé une fois que l’on a compris son usage.
Imaginez : vous travaillez dans votre terminal et vous avez besoin d’empêcher l’affichage des messages d’erreur dans la console. Mais ce n’est qu’une de ses utilités. Si vous avez un peu d’expérience en programmation, vous savez qu’un objet de type null est souvent utile pour signifier « rien », car 0 ou une chaîne vide '' sont des valeurs qui ont un sens pour l’ordinateur.

/dev/null est un fichier spécial de type character device qui ignore tout ce qu’on y écrit et renvoie immédiatement une fin de fichier (EOF) lorsqu’on tente de le lire.

Exemple : chercher un fichier nommé secret.txt dans toute l’arborescence sans afficher les erreurs de permission.
```bash
    find / -name secret.txt 2>/dev/null
```
Ici, 2> redirige la sortie d’erreur standard (descripteur de fichier 2) vers /dev/null, ce qui fait disparaître silencieusement tous les messages indésirables.

## Oh non, c’est plein

`/dev/full`

Un autre fichier étrange. Imaginez un fichier qui est toujours plein : vous ne pouvez rien y écrire. Inutile ? Pas du tout. Il est précieux pour les développeurs qui souhaitent tester la réaction de leur programme face à une erreur « disque plein ».

/dev/full est un fichier de périphérique qui renvoie toujours l’erreur ENOSPC (No space left on device) lors d’une tentative d’écriture, mais qui peut être lu pour retourner une suite de zéros (comme /dev/zero). Essayez la commande suivante pour constater l’erreur :
```bash
    echo "Bonjour" > /dev/full
```

## Aléatoire

`/dev/random` et `/dev/urandom`

Vous avez besoin de nombres aléatoires pour vos programmes de cryptographie ou vos simulations ? C’est là que ces deux fichiers entrent en jeu. L’ordinateur étant une machine déterministe, il ne peut pas produire de « vrai » hasard sans une source extérieure.

Précision technique :

    /dev/random : puise dans une réserve d’entropie (bruit matériel, frappes clavier, mouvements de souris). Si la réserve est vide, la lecture bloque jusqu’à ce que suffisamment d’entropie soit collectée.

    /dev/urandom (unlimited random) : ne bloque jamais. Même quand l’entropie est faible, il utilise un générateur pseudo-aléatoire cryptographique régulièrement réensemencé. Sur les systèmes Linux modernes, c’est la source recommandée pour la plupart des usages cryptographiques.

## Deux sorties pour une entrée

`/dev/stdin`, `/dev/stdout` et `/dev/stderr`

Quand on voit la complexité de la gestion des flux d’entrée et de sortie dans d’autres environnements, on pourrait s’attendre à des mécanismes très lourds. Sous Linux, tout se résume à ces trois fichiers, qui sont en réalité des liens symboliques vers les descripteurs du processus courant.

* /dev/stdin : lien vers /proc/self/fd/0, correspond à l’entrée standard (généralement le clavier, sauf redirection).

* /dev/stdout : lien vers /proc/self/fd/1, sortie normale des résultats (console par défaut).

* /dev/stderr : lien vers /proc/self/fd/2, sortie réservée aux messages d’erreur (également la console par défaut, mais permet de séparer les flux).

## Beaucoup trop de zéros

`/dev/zero`

Imaginez un fichier capable de déverser un flot infini de zéros, pouvant faire grimper la taille de vos fichiers à plusieurs gigabits en quelques secondes. C’est exactement ce que fait /dev/zero.
Ce fichier spécial est souvent utilisé avec la commande dd pour créer des fichiers vides de taille définie ou pour effacer des données (par exemple dd if=/dev/zero of=/dev/sda pour écraser un disque, à manipuler avec précaution).


## Drôle de description

Pour bien comprendre le comportement de /dev/null, /dev/stdout ou les redirections, il faut aborder une notion fondamentale : les descripteurs de fichiers.

Un descripteur de fichier est un simple entier positif que le noyau Linux attribue à chaque fichier, pipe, socket ou périphérique ouvert par un processus. Il sert de « ticket » ou de référence pour que le programme puisse lire ou écrire dans ce flux.

Les trois descripteurs standards :
Chaque processus lancé dans un terminal hérite automatiquement de trois canaux de communication ouverts :

|Descripteur|	Nom symbolique|	Rôle|
|0	|stdin	|Entrée standard (lecture, venant du clavier par défaut)|
|1	|stdout	|Sortie standard (affichage normal des résultats)|
|2	|stderr	|Sortie d’erreur standard (messages d’erreur)|

C’est grâce à ces numéros que l’on peut réaliser les redirections vues précédemment :

    1>fichier redirige la sortie standard (stdout) vers un fichier.

    2>fichier redirige la sortie d’erreur (stderr) vers un fichier.

    2>&1 redirige la sortie d’erreur vers le même endroit que la sortie standard.

Exemple pratique :
```bash
    ls /dossier_existant /inexistant > resultats.txt 2>&1
```
Ici, stdout (1) est redirigé vers resultats.txt, et stderr (2) est redirigé vers l’emplacement actuel de stdout (&1). Ainsi, la liste du dossier existant et le message d’erreur pour le dossier inexistant se retrouvent tous deux dans le même fichier.

Cette simplicité apparente (trois petits numéros) est l’un des piliers de la puissance et de la flexibilité de la ligne de commande Linux.