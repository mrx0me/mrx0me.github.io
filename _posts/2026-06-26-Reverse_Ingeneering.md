---
layout: post
title: L'ingenierie inverse ou l'art de comprendre le machine (langange machine)
image: "/assets/images/re/image.jpg"
category: Reverse Ingenering
author: mr0me
---

Parmi les compétences qui existent dans ce vaste univers qu'est la cybersécurité, nous avons l'ingénierie inversée, autrement appelée la rétro-ingénierie, de l'anglais *Reverse Engineering*. C'est une méthode qui tente d'expliquer, par déduction et analyse systémique, comment un mécanisme, un dispositif, un système ou un programme existant accomplit une tâche, sans connaissance précise de la manière dont il fonctionne. Au sens large, il peut aussi s'agir d'une approche d'ingénierie qui part des performances (ou de critères cibles) d'un dispositif pour déterminer, à l'aide d'une intelligence artificielle le cas échéant, la configuration optimale permettant ce résultat, renversant ainsi le processus traditionnel de conception qui part d'une idée de design pour ensuite en vérifier le rendement. La rétro-ingénierie s'applique notamment dans les domaines de l'ingénierie mécanique, informatique, électronique, chimique, biologique et dans celui du design. (Wikipédia)

Nous prendrons cette définition dans le domaine de l'informatique (plus précisément de la programmation). Si vous êtes initié à la programmation, vous sauriez qu'il existe deux types de langages de programmation :

- Les langages interprétés, qui ne sont pas directement exécutés par le processeur ;
- Les langages compilés, qui sont directement exécutés par le processeur.

Les langages dits interprétés sont lents, mais une fois que vous avez écrit du code, vous n'avez plus besoin de le modifier pour l'exécuter ailleurs, sur une machine différente de la vôtre, à l'aide de l'interpréteur. C'est le cas des langages comme le *Python*, *JavaScript*, *Java*, etc.

Tandis que les langages compilés sont ceux qui sont traduits en langage machine, une suite de 0 et de 1 pratiquement incompréhensibles pour nous autres humains (sauf si vous y arrivez), pour être directement exécutés par le processeur. Vous reconnaîtrez ces programmes par les extensions .exe, .bin, .elf.

Vous l'aurez compris : si vous souhaitez partager votre programme écrit dans un langage interprété, vous devrez aussi partager votre code source (le code que vous avez écrit). En revanche, pour un langage compilé, vous devez traduire votre code source en langage machine de la machine sur laquelle vous souhaitez exécuter votre programme. Pour s'assurer que personne ne soit laissé de côté, il faudra compiler votre programme pour plusieurs types de processeurs afin d'assurer la compatibilité.

L'ingénierie inversée de programmes informatiques ne concerne pas seulement les programmes interprétés, dont vous pouvez aisément avoir accès au code source et en modifier le fonctionnement à la volée, mais aussi les programmes compilés en langage machine. Dans cet article, je parlerai uniquement de l'ingénierie inversée de programmes compilés.

L'ingénierie inverse des programmes informatiques est souvent appliquée pour contrer la propagation de logiciels malveillants par l'analyse de leur comportement et de leurs activités, ou encore par les développeurs durant le débogage pour régler des soucis ou optimiser le fonctionnement du programme.

Dans le but de vous en démontrer l'utilité, nous ferons une petite modification d'un programme compilé. Notre cible sera le célèbre *Hello, World!* en langage *C* :

```C
#include <stdio.h>
int main(){
    printf("Hello , World!\n");
    return 0;
}
```

Sous Linux, vous n'avez qu'à copier ce code et le compiler avec un compilateur comme *GCC*.

![image de la compilation et execution](/assets/images/re/compilation.png)

En exécutant votre exécutable, vous pouvez voir que le programme fonctionne comme prévu. Imaginez que vous souhaitiez changer le texte sans avoir en votre possession le code source. Impossible ? Pas si sûr, mais autant mieux vous prévenir qu'une compréhension du langage C et de l'assembleur, même élémentaire, sera nécessaire.

Nous allons modifier notre programme pour qu'il affiche "HACK THE WORLD" à la place de "Hello, World!". Nous allons commencer par localiser le texte à modifier en utilisant le débogueur GDB. Dans notre cas, nous voyons que le texte est situé à l'adresse 0x2004 (cela pourrait ne pas être le cas chez vous, veillez à vous en assurer). Ne vous inquiétez pas si vous ne comprenez pas ce charabia, il s'agit de l'assembleur (précisément l'assembleur AT&T x86). Vous trouverez un lien vers d'excellentes ressources pour comprendre cette bizarrerie.

![Debogueur](/assets/images/re/debug.png)

Maintenant que nous connaissons l'emplacement du texte, il suffit de le modifier, et cela ne se fait pas de n'importe quelle façon, puisque les instructions sont représentées en hexadécimal. Nous utiliserons un éditeur hexadécimal (oui, oui, un programme qui vous permet d'écrire en hexadécimal). Dans mon cas, j'utilise hexedit, et je me rends à l'adresse du texte.

![localiser le texte ](/assets/images/re/trouve_le_text.png)

Allez sur le site [CyberChef](https://gchq.github.io/CyberChef/#recipe=To_Hex('Space',0)&input=SEFDSyBUSEUgV09STEQh) pour encoder votre message en hexadécimal.

![CyberChef encodage](/assets/images/re/cyberchef_encode.png)

Remplacez l'ancien texte par le nouveau et sauvegardez.

![modification](/assets/images/re/version_modifie.png)

C'est fini, vous êtes parvenu à modifier le texte affiché par un programme sans toucher au code source.

![Resultat](/assets/images/re/resultat.png)

Je suis bien conscient que beaucoup des concepts liés à ce que je viens d'aborder n'ont pas été suffisamment traités dans cet article. Le but de l'article était plutôt informatif : juste vous dire que c'est possible.

## Ressources
[Langage assembleur](https://repository.root-me.org/Reverse%20Engineering/FR%20-%20Reverse%20Engineering%20pour%20D%C3%A9butants%20-%20Dennis%20Yurichev.pdf)  
[Langage C](https://zestedesavoir.com/tutoriels/pdf/755/le-langage-c-1.pdf)  
[Architecture des ordinateurs](https://apprendre.modulo-info.ch/archi/index.html)

