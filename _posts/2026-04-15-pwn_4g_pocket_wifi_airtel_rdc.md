---
layout: post
title: "PWN : 4G POCKET WIFI AIRTEL RDC"
image: "assets/images/router_airtel_1/4g_pocket_wifi_airtel.png"
category: pwn
author: mr0me
---

**AVERTISSEMENT** : Cette publication est à but informatif et n'est nullement mise en ligne pour inciter quiconque à enfreindre la loi. Si vous souhaitez refaire l'expérience de ce qui est décrit dans cet article, alors vous devez le faire sur un équipement dont vous disposez des droits pour le faire.

**NOTE** : La vulnérabilité présentée dans cet article est une vulnérabilité 0-day. L'entreprise qui le commercialise a été contactée, mais aucune réponse de leur part n'a été reçue depuis maintenant un an.

## Intro

Les IoT (Internet des objets) sont des appareils fascinants, captivants et impressionnants pour la plupart des gens, mais ils sont assez effrayants quand on se penche sur la sécurité mise en place sur ces appareils.

L'un des grands soucis avec ces appareils, qui deviennent de plus en plus petits et qui sont en mesure d'offrir connectivité et échange d'informations, est qu'ils ont le plus souvent des ressources limitées. C'est-à-dire que, par exemple, pour garantir une bonne autonomie énergétique, ils doivent s'assurer de ne pas trop consommer de ressources. Et pour cela, dans la phase de conception, certaines options peuvent être sacrifiées de sorte que si le produit fonctionne, tout le reste ne compte plus. Ainsi, on pourrait se retrouver devant un cas où le firmware ne doit pas dépasser une certaine taille, ce qui signifie que certaines dépendances du projet doivent être supprimées pour utiliser à la place une solution rafistolée qui n'a juste besoin que de fonctionner. On cherche à concevoir le projet à moindres coûts, même si la qualité n'est pas la même, du moment que cela fonctionne (cela touche aussi d'autres domaines).

Le but de cette publication n'est pas de parler des problèmes des IoT en général, mais plutôt d'un de ceux que j'ai pu analyser : il s'agit du routeur 4G POCKET WIFI commercialisé par Airtel RDC.
Exploitation

La faille de sécurité découverte dans cet appareil concerne son interface d'administration, et cette faille n'est que cela ; pourtant, le problème fait tomber toute l'infrastructure.

Cette faille est de type Broken access control, traduite en français par « contrôle d'accès brisé », ce qui, en termes moins compliqués, signifie que nous sommes parvenus à interagir avec le système sans aucun contrôle d'accès.

En sécurité informatique, nous trouvons deux mots qui parfois prêtent à confusion : il s'agit de l'authentification et de l'autorisation.

L'authentification consiste à connaître l'identité de celui qui interagit avec le système, tandis que l'autorisation consiste à savoir si la personne a le droit de mener cette action.

Comme la plupart des routeurs, celui-ci intègre aussi un mécanisme d'authentification et d'autorisation pour accéder à l'administration du routeur, sauf que ce mécanisme est si mal implémenté qu'il est même inexistant.

![login page](/assets/images/router_airtel_1/capture1.png)

En essayant de consulter certaines pages qui sont censées être d'accès restreint mais qui ne le sont pas, on se rend compte que l'authentification est factice : la vérification ne s'effectue que sur le front-end, et si nous arrivons à suivre le chemin que le fabricant voulait que nous suivions, un jeton (cookie) nous serait attribué, mais même ce jeton n'est pas vérifié, ce qui signifie que nous pouvons nous en attribuer un nous-mêmes et cela fonctionnera sans souci :]

Comme conséquence, nous pouvons :

    Voir et modifier le mot de passe de l'administration et celui du Wi-Fi

    Voir et envoyer les SMS

    Lister les appareils connectés

    Exécuter des codes USSD

    Éteindre et réinitialiser la mémoire du routeur

    Installer un firmware infecté

    Rendre le routeur inutilisable (absence d'option de réinitialisation par un bouton physique)

Pouvant effectuer toutes ces manipulations, dans cette publication nous allons juste démontrer la possibilité de voir le mot de passe administrateur ; les autres actions peuvent être réalisées depuis l'interface d'administration.

![demo](/assets/images/router_airtel_1/demo.png)

Un exploit est disponible [ici](https://github.com/).


