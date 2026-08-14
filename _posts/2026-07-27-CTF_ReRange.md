---
layout: post
title: "CTF : ReRange" 
image: "https://rerange.org/wp-content/uploads/2025/09/RR_logo-300x292.png"
category: ctf
author: Me
---

# Présentation
Préparez-vous à déchirer les couches de protection et à plonger au cœur de la logique binaire ! Ce write-up retrace mon parcours à travers le CTF [ReRange](https://rerange.org/challenges/), une arène qui propose pas moins de **18 défis** de reverse engineering. Sur ce champ de bataille numérique, j'ai triomphé de **11 d'entre eux**, extrayant des flags cachés dans des exécutables coriaces, des machines virtuelles obscurcies et des algorithmes XOR retors.

Un **12e défi** m'a pourtant joué un tour : le flag s'affichait fièrement à l'écran, mais le dashboard le rejetait obstinément ! Une énigme de plus que j'espère élucider prochainement dans une mise à jour de ces notes.

Pour cette chasse aux trésors binaires, mon arsenal se compose de **GDB**, **objdump**, **Ghidra** et **Python**, avec un focus principal sur les fichiers au format ELF. Enfilez votre casque de décompilation, c'est parti !

# Challenges

## Dropper 1 (C)
L'indice nous dit : « Cet exécutable en contient un autre. La logique du flag se trouve dans le programme embarqué. Extrayez-la. »

Nous importons le fichier dans un projet sur Ghidra pour décompiler le code afin d'avoir une idée de ce que le programme fait. Nous pouvons clairement voir que pour parvenir à extraire le programme inclus dans le premier, il suffit de saisir un argument de plus de 128 caractères.
![image code decompiler](/assets/images/rerange/dropper_1_1.png)

Nous exécutons notre binaire avec la commande **./dropper_1 `python3 -c "print('1'*129,'\n')"`** pour passer l'argument, puis nous listons le contenu du répertoire pour voir apparaître un autre binaire que nous examinons à nouveau dans Ghidra.

La fonction entry de ce second binaire appelle une fonction dont le code décompilé est :

![second binaire](/assets/images/rerange/dropper_1_2.png)

La méthode la plus simple consiste à copier ce code et à insérer un simple `printf` à la ligne appropriée.

Après avoir légèrement modifié notre code, il ressemblera à ceci :

![code flag](/assets/images/rerange/dropper_1_3.png)

Ce code effectue simplement une opération XOR sur un ensemble de valeurs et nous affiche le flag.

![flag](/assets/images/rerange/dropper_1_flag.png)

## Decompyle Me
L'indice dit : « Saviez-vous que certains exécutables Python peuvent être décompilés pour retrouver le code source original ? pyinstxtractor-ng offre un moyen simple et rapide d'extraire les fichiers d'une archive PyInstaller. »

Premièrement, nous devons extraire le script Python qui a été compilé en exécutable en utilisant [pyinstxtractor](https://github.com/extreamecoders-re/pyinstxtractor). Un nouveau répertoire devrait apparaître, contenant un fichier `secure.pyc` qui est du Python compilé. Pour le décompiler, nous pouvons utiliser le site [www.decompiler.com](https://www.decompiler.com/python) qui donne ce petit code :

![python decompile code](/assets/images/rerange/decompyle_me_1.png)

Il nous suffit de décoder la chaîne de caractères et nous obtiendrons le flag.

![python decompile flag](/assets/images/rerange/decompyle_me_flag.png)

## Easy GZ
L'indice nous dit : « Je connais la réponse au sens de la vie. Je ne sais pas pourquoi ce script continue de me dire que j'ai tort. Il doit y avoir un bug ; peux-tu le corriger ? Cela m'a complètement démoralisé. »

Après avoir décompressé le fichier zip, nous obtenons un fichier Python qui est obscurci. Il est assez simple : nous allons remplacer la fonction `exec` par `print` pour voir le code qui sera exécuté. Nous pouvons lire le flag dans le code source.

![python stragger](/assets/images/rerange/2_stragger_flag.png)

## Long Timer
L'indice dit : « Vous pouvez avoir le drapeau… après avoir attendu 1 215 752 191 secondes. »

![long timer assembleur](/assets/images/rerange/long_timer_assembly.png)
En lisant le code assembleur de ce binaire, nous découvrons que, comme le dit l'indice, le programme attend durant une très longue période avant de nous afficher le flag. Nous allons donc devoir sauter cette partie d'attente et appeler directement la fonction flag pour lire le flag.

![long timer assembleur](/assets/images/rerange/long_timer_flag.png)

## Fake Checks 1
L'indice dit : « Ce programme se termine quoi qu'il arrive ; peux-tu patcher les vérifications ? Utilise les chaînes de caractères. »

En visualisant la structure d'exécution de notre binaire avec Ghidra, nous obtenons le résultat suivant :
Il s'agit d'un ensemble de fonctions qui font de fausses vérifications et appellent la fonction `exit` pour tuer le processus.
Nous allons mettre un point d'arrêt sur la fonction `exit` et sauter à la partie du code qui affiche le flag.

![structure](/assets/images/rerange/fake_check_1.png)

Nous utilisons `gdb` pour cela, et les adresses de saut ont été obtenues grâce à Ghidra.

![flag gdb](/assets/images/rerange/fake_check_flag.png)

## Single Byte XOR
Indice : « Ce programme vérifie votre saisie en "déchiffrant" le texte chiffré codé en dur. »

Il s'agit d'un programme qui reçoit une entrée utilisateur, y effectue une opération XOR et la compare à la valeur cryptée inscrite dans le code.
La manière la plus simple pour trouver le flag, pour moi, a été de copier le code décompilé avec Ghidra, d'y apporter des modifications, puis de le compiler pour lire le flag.

Dans ce [fichier](/assets/code/Rerange/single_byte_xor.txt), vous trouverez la sortie décompilée de Ghidra, et la source modifiée qui affiche le flag se trouve dans [cet autre fichier](/assets/code/Rerange/single_byte_xor.c).

![flag](/assets/images/rerange/single_byte_xor_1.png)

## Schrodinger’s Binary 1
Indice : « Pour exécuter ce fichier, utilisez un débogueur. Pour faire planter ce fichier, utilisez un débogueur. Contournez la technique anti-débogage. »

En analysant ce binaire avec Ghidra, nous pouvons voir que si le programme détecte un débogueur, il plante ; s'il n'en détecte pas, il plante aussi, puis appelle la fonction flag qui affiche le flag.
Pour reconstruire le flag, j'ai simplement recréé la partie du code qui gère le flag, mais il est aussi possible de résoudre ce défi en utilisant des outils comme Wingdb.

![cat main decompile](/assets/images/rerange/cat_1_main.png)

![cat flag code ](/assets/images/rerange/cat_1_flag.png)
Dans le code de flag, nous voyons qu'il utilise une chaîne de caractères stockée en mémoire, effectue des opérations dessus, puis affiche le flag. Nous pouvons copier le tableau qui contient la valeur cryptée du flag, reconstruire le programme et le compiler (le code se trouve à [cet emplacement](/assets/code/Rerange/)).

![schrodinger flag](/assets/images/rerange/flag_schrondiger.png)

## Virtualized 1
Indice : « This program executes a small virtual machine. Understand the instruction set to recover the flag. »

La virtualisation est l'une des techniques d'obfuscation qui consiste à placer un intermédiaire entre la vraie charge utile (le code que nous voulons exécuter) et le matériel. Le code que nous voulons exécuter n'est pas directement exécuté par le processeur, mais est considéré comme des données qui seront modifiées par le lanceur, puis exécutées pas à pas.

Si vous exécutez le programme, vous verrez que la sortie ne nous donne pas grand-chose comme réponse.
![virtualized 1 output](/assets/images/rerange/virtualized_output.png)

Nous allons l'analyser avec Ghidra en l'important, et nous pouvons trouver cette partie assez étrange.
Nous voyons que la VM est appelée avec comme argument une valeur bizarre qui est stockée dans le programme.

![image virtualized 3](/assets/images/rerange/virtualized_1.png)

La valeur est parcourue et décodée dans la boucle ; chaque instruction est passée à une autre fonction finale.
![image virtualized 4](/assets/images/rerange/virtualized_2.png)
Cette fonction finale ne fait rien de spécial et nous pouvons en déduire que le flag est la valeur de param_2.
![image virtualized 5](/assets/images/rerange/virtualized_3.png)
Il faut maintenant poser une sonde sur cette fonction pour obtenir la valeur du flag, mais nous aurions pu aussi reconstruire ce programme ; ici nous n'en avons pas besoin.
Pour poser la sonde qui nous donnera la valeur du flag, nous utiliserons `gdb`. Avec Ghidra, nous voyons à quelle adresse se trouve cette instruction pour y placer un point d'arrêt et afficher le registre qui contiendra notre flag.

![virtualized assembly flag](/assets/images/rerange/virtualized_4.png)

Nous pouvons copier cette sortie de gdb et écrire un petit script pour obtenir la correspondance de ces valeurs dans la table ASCII :

    (gdb) break *0x0000555555555050+304
    Breakpoint 2 at 0x555555555180
    (gdb) commands 
    Type commands for breakpoint(s) 2, one per line.
    End with a line saying just "end".
    >print $cl
    >continue
    >end
    (gdb) run
    Breakpoint 2, 0x0000555555555180 in ?? ()
    $4 = 82
    ...
    $34 = 86
    VM execution finished.

Et avec ce simple script Python, nous pouvons retrouver le flag :

```python
flag_encode = [82, 101, 82, 97, 110, 103, 101, 123, 118, 109, 95, 53, 116, 97, 116, 51, 95, 110 ,48, 55, 95, 53, 116, 100, 48, 117, 116, 95,100,53,86 ]

for i in flag_encode:
    print(chr(i),end='')

print('\n')
```
Le flag est celui-ci, mais je ne sais pour quelle raison il n'est pas validé par le tableau de bord :
> ReRange{s1ngl3by73_0bfusc4t0r_d}

## Ransim 1
Indice : « Reverse this ransomware sim and derive the key for the decryptor. Use it to decrypt the flag file. »

Celui-ci est un peu plus complexe : nous avons affaire à un binaire qui a déjà crypté le flag dans le fichier `ctf_flag/flag.txt.locked` et utilise l'algorithme de chiffrement AES, qui est un algorithme symétrique. La clé de chiffrement et celle de déchiffrement sont donc les mêmes. Notre but est donc d'extraire la clé et d'utiliser le fichier Python fourni pour déchiffrer le flag.

![](/assets/images/rerange/ransim_1.png)

Premièrement, nous devons comprendre comment fonctionne le binaire. Pour cela, j'ai commenté la sortie de Ghidra. Nous voyons avec mes commentaires une fonction qui génère la clé, et notre but est de pouvoir la capturer. Nous utilisons Ghidra pour savoir où placer le point d'arrêt, et nous cherchons sur la pile la clé qui est posée suivant cette capture, donc `rbp-112`.

![](/assets/images/rerange/ransim_2.png)

    (gdb) break *0x0000555555555120+3424
    Breakpoint 2 at 0x555555555e80
    (gdb) run
    Starting program: /home/oem/Bureau/ReRange/ransim_1_dist_linux/ransim_1/ransim_1 
    [Thread debugging using libthread_db enabled]
    Using host libthread_db library "/lib/x86_64-linux-gnu/libthread_db.so.1".

    Breakpoint 2, 0x0000555555555e80 in ?? ()

    (gdb) info r
    rax            0x7fffffffdb10      140737488345872
    rbx            0x0                 0
    rcx            0x5                 5
    rdx            0x55555555a490      93824992257168
    rsi            0x7fffffffdb00      140737488345856
    rdi            0x7fffffffdaf8      140737488345848
    rbp            0x7fffffffdb80      0x7fffffffdb80
    rsp            0x7fffffffdae0      0x7fffffffdae0
    r8             0x5555555592a0      93824992252576
    r9             0x555555559480      93824992253056
    r10            0x55555555a480      93824992257152
    r11            0x530ea5639425ef59  5984902801927827289
    r12            0x7fffffffdc98      140737488346264
    r13            0x555555555cb5      93824992238773
    r14            0x555555557dd8      93824992247256
    r15            0x7ffff7ffd040      140737354125376
    rip            0x555555555e80      0x555555555e80
    eflags         0x246               [ PF ZF IF ]
    cs             0x33                51
    ss             0x2b                43
    ds             0x0                 0
    es             0x0                 0
    fs             0x0                 0
    gs             0x0                 0

    (gdb) x/32x 0x7fffffffdb80-112 
    0x7fffffffdb10:	0x4e	0x9b	0x5b	0x10	0xdb	0xd3	0x3a	0xef
    0x7fffffffdb18:	0x3b	0x11	0xc1	0x1e	0x9c	0x11	0x92	0x7d
    0x7fffffffdb20:	0xc1	0x18	0x06	0x94	0x3f	0xfc	0x95	0xb3
    0x7fffffffdb28:	0x36	0xed	0x62	0x32	0x32	0x17	0x00	0x21

Il suffit maintenant de regrouper la clé en valeurs hexadécimales ou leurs correspondances ASCII, et de l'utiliser comme clé dans le programme Python de déchiffrement.
![ransim flag](/assets/images/rerange/ransim_flag.png)

## XOR Warmup
Indice : « I recovered this binary from a recent security incident. It needs a password, can you crack it? »

Ce défi est vraiment très facile, bien qu'il soit placé dans la catégorie *medium*. Il effectue simplement une opération XOR sur une entrée utilisateur et la compare à ce qu'il a comme flag obscurci.
Pour le résoudre, il faut d'abord connaître le fonctionnement de la fonction XOR. C'est une opération logique telle que `x XOR y = z` ; si nous disposons de `y` et `z`, nous pouvons retrouver `x`.
La sortie du programme décompilé avec Ghidra nous donne :

    undefined8 main(void){
    char *pcVar1;
    undefined8 uVar2;
    byte local_98 [128];
    size_t local_18;
    ulong local_10;
    
    local_98[0] = 0;
    ...
    local_98[0x7f] = 0;
    printf("Enter flag: ");
    fflush(stdout);
    pcVar1 = fgets((char *)local_98,0x80,stdin);
    if (pcVar1 == (char *)0x0) {
        uVar2 = 1;
    }
    else {
        local_18 = strcspn((char *)local_98,"\r\n");
        local_98[local_18] = 0;
        if (local_18 == 0x20) {
        for (local_10 = 0; local_10 < 0x20; local_10 = local_10 + 1) {
            if ((byte)(local_98[local_10] ^ "sundaeEnter flag: "[local_10 % 6]) != obf_flag[local_10]) {
            puts("Nope.");
            return 0;
            }
        }
        puts("Correct!");
        uVar2 = 0;
        }
        else {
        puts("Nope.");
        uVar2 = 0;
        }
    }
    return uVar2;
    }

J'ai trouvé plus simple de reconstruire cette partie du programme et de la modifier pour retrouver le flag. Nous avons donc le petit programme suivant que nous allons compiler :

```c
    #include <stdio.h>
    #include <stdint.h>

    int main(void){
    char *pcVar1;
    int uVar2;
    uint8_t local_98 [128];
    size_t local_18;
    unsigned long local_10;
    int obf_flag [] =   { 0x21, 0x10, 0x3c, 0x05, 0x0f, 0x02, 0x16, 0x0e, 0x16, 0x54, 0x13, 0x3a, 0x04, 0x41, 0x1c, 0x09, 0x14, 0x15, 0x2c, 0x4c, 0x08, 0x57, 0x00, 0x52, 0x10, 0x47, 0x0b, 0x55, 0x03, 0x50, 0x46, 0x08 };
    
    local_98[0] = 0;
    ...
    local_98[0x7f] = 0;
    char value;
    for (local_10 = 0; local_10 < 0x20; local_10 = local_10 + 1) {
        value = "sundaeEnter flag: "[local_10 % 6] ^ obf_flag[local_10];
        printf("%c",value);
        }
    return 0;
    }
```
Ce qui nous donne le flag après compilation :
![xor warmup flag](/assets/images/rerange/xor_warmup_flag.png)

## Fake Checks 2
Indice : « This program terminates no matter what, can you patch the checks and get the flag? »

La première fois que j'ai résolu celui-ci, j'avais simplement reconstruit le binaire sans les appels à `exit` qui nous empêchent de voir le flag. Une autre technique que nous pouvons utiliser consiste à injecter une bibliothèque en remplaçant la fonction `exit` par notre propre fonction qui ne met pas fin au programme.
Mais ici, nous allons plutôt patcher le programme, donc modifier un peu le binaire sans le reconstruire complètement. En lisant le code assembleur de notre défi, nous découvrons qu'il n'y a qu'une seule fonction qui appelle `exit` ; nous allons donc l'empêcher de le faire.
![fake check exit assembly](/assets/images/rerange/fake_check_2_exit.png)

Nous voyons les opcodes `E8 BB FE` (call exit) qui correspondent à un appel à la fonction `exit`. Nous allons les chercher et les modifier en utilisant `hexedit`.
![](/assets/images/rerange/fake_check_2_hexedit.png)
Nous remplaçons ces opcodes par `90 C9 C3` (nop leave ret), ce qui fera que notre fonction ne fera rien. Il suffit de sauvegarder et d'exécuter. Attention à ne modifier que cette partie-là.
![](/assets/images/rerange/fake_check_2_flag.png)

## Virtualized 2
Indice : « Understand the instruction set this VM uses to recover the flag. Fake instructions, misleading state changes, and a shifting key. »
Ce défi est similaire au précédent sur la virtualisation. Nous commençons par déterminer l'emplacement où nous présumons que l'exécution s'effectue, et nous y posons un point d'arrêt pour lire la valeur du registre qui nous intéresse.

    (gdb) break *0x0000555555555050+411
    Breakpoint 2 at 0x5555555551eb
    (gdb) commands 
    Type commands for breakpoint(s) 2, one per line.
    End with a line saying just "end".
    >print $cl
    >continue 
    >end
    (gdb) run
    Breakpoint 2, 0x00005555555551eb in ?? ()
    $33 = 82
    Breakpoint 2, 0x00005555555551eb in ?? ()
    ...
    Breakpoint 2, 0x00005555555551eb in ?? ()
    $57 = 51

Nous copions ces valeurs pour retrouver la représentation ASCII correspondante en utilisant Python :

```python
flag = [ 82, 101, 82, 97, 110, 103, 101, 123, 118, 109, 95, 115, 116, 52, 116, 51, 95, 100, 114, 49, 102, 116, 95, 109, 51, 100, 49, 117, 109, 95, 97, 125]

for i in flag:
    print(chr(i),end='')
```
![virtualized 2 flag](/assets/images/rerange/virtualized_2_flag.png)

___

Et voilà ! Ce tour d’horizon des défis ReRange touche à sa fin, mais pas notre soif d’apprendre. Chaque binaire retourné, chaque XOR inversé et chaque VM domptée m’a offert une nouvelle pièce du puzzle de l’ingénierie inverse. J’espère que ces lignes vous auront donné quelques astuces, et peut-être même l’envie de vous frotter à ces exécutables coriaces.

N’hésitez pas à fouiller, à patcher, à décompiler… et à échouer aussi, car c’est ainsi qu’on progresse. Si je parviens à débloquer le mystère du 12e flag, je mettrai à jour ces notes. En attendant, bonne chance dans vos propres quêtes, et n’oubliez pas : le flag est souvent caché là où on ne l’attend pas !

À bientôt dans les méandres du code.
