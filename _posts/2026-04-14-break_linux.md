---
layout: post
title: L'art de detruire sa distribution Linux
image: "https://static0.howtogeekimages.com/wordpress/wp-content/uploads/2025/06/linux-mascot-broken-terminal.png?q=70&fit=crop&w=1600&h=900&dpr=1"
category: Linux
author: mr0me
---

Commandes Linux à ne jamais exécuter (ou avec une extrême prudence)

Après une petite période d'utilisation de Linux comme système d'exploitation à temps plein en choisissant l'une de ses distributions, on finit par connaître et comprendre quoi ne pas exécuter comme commandes. Souvent l'exécution de ces commandes est le fruit d'une erreur de l'opérateur, sauf si c'est volontaire.

Il est à rappeler que ces commandes seront exécutées avec les permissions de l'utilisateur root (généralement via sudo). Sans ces privilèges, la plupart d'entre elles échoueront avec un simple message « Permission denied », ce qui est heureusement une protection efficace pour les novices.


# Permission dangereuse

`chmod 777 -R /`

Dans la catégorie des commandes à ne jamais exécuter figure celle-ci. La commande chmod est utilisée pour modifier les permissions sur des fichiers sous Linux (rappelons que sous Linux tout est fichier, même les répertoires).

Décomposons les paramètres 
    777 : Sous Linux, nous pouvons définir les permissions sur un fichier pour l'utilisateur propriétaire, le groupe auquel il appartient et les autres utilisateurs. Ces permissions sont la lecture (read, code octal 4), l'écriture (write, code octal 2) et l'exécution (execute, code octal 1). On peut attribuer plusieurs permissions en additionnant ces valeurs. Le premier 7 correspond donc à lecture + écriture + exécution pour l'utilisateur qui saisit la commande, le deuxième pour le groupe et le troisième pour les autres utilisateurs du système.

    -R : Cette option signifie récursif. Nous demandons au système d'appliquer ces mêmes permissions à tous les fichiers et sous-dossiers du répertoire indiqué.

    / : Le répertoire racine, qui correspond au début de l'arborescence des fichiers sous Linux. C'est le répertoire parent de tous les autres.

Conséquence : Nous donnons tous les pouvoirs (lecture, écriture, exécution) à tout le monde sur l'intégralité du système. Pire encore, cette commande supprime les bits spéciaux comme setuid ou setgid qui sont indispensables au fonctionnement de programmes critiques tels que sudo ou passwd. Résultat : même après avoir tenté de remettre des permissions normales, le système restera inutilisable et nécessitera une réparation avancée depuis un Live USB.


# Oups ! C'est effacé

`rm -rf / `

La commande rm est utilisée sous Linux pour supprimer un fichier.

    -rf : Combinaison de -r (récursif) pour supprimer les dossiers et leur contenu, et -f (force) pour ne demander aucune confirmation.

    / : Le répertoire à supprimer, correspondant à la racine de l'arborescence.

Précision importante : Sur les systèmes Linux modernes, la commande rm -rf / ne fonctionne plus. Les développeurs ont ajouté une sécurité appelée --preserve-root. Si vous tapez sudo rm -rf /, vous obtiendrez un message d'erreur : « It is dangerous to operate recursively on '/' ... Use --no-preserve-root to override ». Cependant, la variante sudo rm -rf /* (avec une étoile) reste dévastatrice. Elle supprimera tout le contenu visible du disque car l'expansion du * passe outre cette protection.

Conséquence : Tout ce qui se trouve sur votre disque dur est supprimé de manière irréversible, sans passer par la corbeille.

# Destination inconnue

`mv / /dev/null`

La commande mv sert à déplacer ou renommer des fichiers ou des répertoires.

    / : Le répertoire racine à déplacer.

    /dev/null : Un fichier spécial du système souvent appelé trou noir ou poubelle car tout ce qui y est écrit est définitivement ignoré.

Que se passe-t-il vraiment ?
Contrairement à l'idée reçue, cette commande ne fait pas disparaître vos données dans le néant comme on le croit souvent. En réalité, /dev/null est un fichier spécial de type caractère, pas un répertoire. La commande va donc tenter de renommer le répertoire / en /dev/null. Comme le fichier /dev/null existe déjà et qu'il n'est pas un dossier, l'opération échouera avec une erreur sur la plupart des systèmes. Si toutefois la commande réussissait (dans un contexte très particulier ou en forçant l'écrasement avec mv -f), le répertoire racine serait remplacé par un fichier spécial inutilisable. Le système deviendrait immédiatement instable et inamorçable, car plus aucun fichier ni dossier ne serait accessible depuis la racine.

Conclusion : Même si elle est moins immédiatement destructrice que rm -rf /*, c'est une commande absurde qui corrompt le système de fichiers de manière subtile mais tout aussi fatale.

# Fonction maléfique

`:(){ :|:& };:`

Il faut d'abord le dire : ceci n'est pas une commande à proprement parler, mais une suite d'instructions pour définir une fonction (oui, comme on le fait dans les langages de programmation) qui est directement appelable depuis votre terminal.

De la manière dont c'est écrit, cela peut faire peur, mais nous pouvons la réécrire plus clairement ainsi :

    mafonction(){
        mafonction | mafonction &
    }
    mafonction

Ce bout de code crée une rafale de processus en arrière-plan. La fonction s'appelle elle-même deux fois (une en premier plan, une en arrière-plan) et se duplique à l'infini. Conséquence : un pic de consommation de la mémoire et du processeur, conduisant au gel complet de la machine (fork bomb).

Bon à savoir : Il existe une parade simple pour éviter ce type d'attaque : la commande ulimit -u permet de limiter le nombre maximal de processus par utilisateur. Un simple ulimit -u 100 avant de tester (dans une machine virtuelle !) empêchera la prolifération incontrôlée.