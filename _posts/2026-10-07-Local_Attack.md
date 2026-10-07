---
layout: post
title: Attaques sur le réseau local (LAN) : panorama des techniques classiques
image: "https://ipcisco.com/wp-content/uploads/2020/03/cyber-attacks-network-attacks-ipcisco.com_-835x500.png"
category: network
author: mr0me
---

# Introduction
Le plus souvent, on nous donne des recommandations comme éviter de nous connecter à des réseaux Wi-Fi inconnus, utiliser un VPN pour chiffrer notre trafic, ou encore s'assurer que nos communications passent par HTTPS. 

Dans cet article, nous allons voir quelques attaques qui peuvent être menées si nous nous retrouvons sur le même réseau qu'une personne malveillante.
Puisque nous allons parler d'attaques sur le réseau local (LAN), autant commencer par clarifier comment s'effectue la communication sur un LAN, quels protocoles sont utilisés, et préciser certains termes que nous emploierons tout au long de l'article.
# Communication sur le LAN
Un réseau informatique est une interconnexion d'appareils reliés entre eux pour échanger des informations. Ces appareils ne jouent pas tous le même rôle sur le réseau :
* Le routeur fait transiter des paquets d'un réseau à un autre (par exemple, du réseau local vers Internet).
* Le concentrateur (hub) et le commutateur (switch) permettent de faire transiter une communication (une trame) de l'expéditeur vers le destinataire. La différence majeure est que le hub diffuse la trame reçue à tous les appareils connectés, alors que le switch l'envoie uniquement au port du destinataire concerné, grâce à une table interne appelée table CAM, qui associe les adresses MAC aux ports du switch.
* Les points de terminaison (endpoints), comme un ordinateur, un téléphone ou une tablette, sont les appareils qui émettent et reçoivent réellement les données.
Pour communiquer sur ce réseau, chaque appareil doit être identifié de manière unique, donc posséder une identité. C'est là qu'interviennent deux types d'adresses :
* L'adresse IP, une suite de 4 octets séparés par des points (par exemple 192.168.1.7), qui sert à identifier un appareil sur un réseau et à acheminer les paquets entre différents réseaux (c'est l'adresse utilisée pour « router » le trafic, y compris sur Internet).
* L'adresse MAC, propre à la carte réseau de l'appareil et attribuée par son fabricant (par exemple A1:2B:CD:12:89:21). En principe fixe, c'est l'identifiant réellement utilisé pour les communications au sein du réseau local.
Puisque les communications sur le réseau local s'effectuent avec l'adresse MAC, il faut un moyen de faire correspondre une adresse IP à une adresse MAC : c'est le rôle du protocole ARP (Address Resolution Protocol). Un appareil qui veut parler à 192.168.1.1 diffuse une requête ARP (« qui a cette IP ? ») et la machine concernée répond avec son adresse MAC ; cette correspondance est ensuite stockée en mémoire dans ce qu'on appelle la table ARP (ou cache ARP).
Quelques autres protocoles et notions que nous croiserons dans cet article :
* DNS (Domain Name System) : traduit un nom de domaine (comme google.com) en adresse IP, afin que nous n'ayons pas à mémoriser des adresses IP pour naviguer sur le web.
* DHCP (Dynamic Host Configuration Protocol) : attribue automatiquement une adresse IP (et d'autres paramètres comme la passerelle et le ou les serveurs DNS) aux appareils qui rejoignent le réseau.
* LLMNR / NetBIOS (NBT-NS) : des protocoles de résolution de noms utilisés par Windows en complément (ou en secours) du DNS, principalement sur les réseaux locaux.
* VLAN (Virtual LAN) : permet de segmenter logiquement un même réseau physique en plusieurs réseaux distincts, comme s'il s'agissait de réseaux séparés, sans câblage supplémentaire. Le trunk est un lien entre switches qui transporte le trafic de plusieurs VLAN à la fois, chaque trame étant marquée (« taguée ») avec un identifiant de VLAN grâce au standard 802.1Q.
* STP (Spanning Tree Protocol) : protocole utilisé entre switches pour éviter les boucles réseau (qui provoqueraient des tempêtes de diffusion) en désactivant logiquement certains liens redondants. Il élit pour cela un switch « chef d'orchestre » appelé root bridge.
* CDP / LLDP (Cisco Discovery Protocol / Link Layer Discovery Protocol) : protocoles utilisés par les équipements réseau pour s'annoncer et se découvrir entre eux (modèle, OS, VLAN, etc.).
* IPv6 et NDP (Neighbor Discovery Protocol) : IPv6 est la version plus récente du protocole IP, avec des adresses bien plus longues que l'IPv4. NDP y joue un rôle équivalent à celui d'ARP en IPv4 (et, dans une certaine mesure, à celui du DHCP, via les messages Router Advertisement).
* TCP et le « handshake » : avant d'échanger des données, deux machines TCP établissent une connexion via un échange en trois temps (SYN, SYN-ACK, ACK). Cet échange laisse une trace temporaire en mémoire sur le serveur, ce qui peut être exploité lors de certaines attaques de déni de service.
* HTTPS / TLS (ex-SSL) : chiffre les communications web afin qu'un tiers qui intercepte le trafic ne puisse pas en lire le contenu en clair.
* Session et cookie : après authentification sur un site, le serveur attribue généralement un identifiant de session (souvent stocké dans un cookie) qui permet de reconnaître l'utilisateur sans lui redemander son mot de passe à chaque requête.


Avec ces quelques explications, nous pouvons passer aux attaques, en détaillant d'autres éléments si besoin. Les problèmes abordés ici ne sont pas des faiblesses propres à un système d'exploitation ou à un équipement réseau en particulier, mais des faiblesses inhérentes aux standards mêmes qui régissent ces communications (les protocoles).
# Attaques
## 1. ARP Spoofing (empoisonnement de cache)
L'attaquant envoie des réponses ARP falsifiées pour associer sa propre adresse MAC à l'IP de la passerelle (dans le cache de la victime) et à l'IP de la victime (dans le cache de la passerelle). Les deux machines croient alors parler directement l'une à l'autre, alors que tout le trafic transite en réalité par l'attaquant : c'est une attaque de type Man-in-the-Middle (MITM).

Nous pouvons voir dans l'image qui suit la table ARP de notre victime avant l'attaque, et constater que tout va bien : 
![arp table before attack](/assets/images/network_attacks/arp_spoof_before_attack.png)

Et dans celle-ci, nous sommes parvenus à modifier la table ARP de la victime, de telle sorte que si elle souhaite communiquer avec l'hôte 192.168.56.1, elle communiquera en réalité avec nous. Nous pouvons ensuite, par discrétion, rediriger son trafic vers le destinataire initial afin de surveiller la communication sans l'interrompre : arp table afte attack

![](/assets/images/network_attacks/arp_spoof_afte_attack_table.png)

## 2. ARP Flooding (saturation de la table CAM)
Contrairement au spoofing ciblé, le flooding (aussi appelé MAC flooding) consiste à envoyer un très grand nombre de trames avec des adresses MAC sources aléatoires. La table CAM du switch, de taille limitée, finit par être saturée. Certains switches basculent alors en mode « fail-open » et diffusent le trafic sur tous les ports, comme un simple hub, ce qui permet ensuite d'écouter passivement tout le trafic du segment.

## 3. DNS Spoofing / Cache Poisoning
Le DNS Spoofing consiste à répondre à une requête DNS avec une adresse IP falsifiée, pour rediriger la victime vers un serveur contrôlé par l'attaquant (phishing, interception de session...). Il existe deux variantes principales : répondre plus vite que le vrai serveur DNS lors d'une attaque MITM (souvent couplée à un ARP spoofing), ou empoisonner le cache d'un résolveur DNS pour affecter plusieurs victimes à la fois.
Ettercap/ifconfig
Ici, dans notre illustration, nous avons usurpé l'adresse de notre serveur DNS, qui a pour adresse IP 192.168.56.3 

![](/assets/images/network_attacks/dns_query_response_spoof.png)

## 4. LLMNR / NBT-NS Poisoning
Sur les réseaux Windows, quand la résolution DNS classique échoue (faute de frappe, nom de partage local...), les machines tentent une résolution de secours en diffusion via LLMNR (Link-Local Multicast Name Resolution) ou NBT-NS (NetBIOS). Un attaquant à l'écoute peut répondre « je suis la ressource demandée » : la victime lui envoie alors directement son hash NTLM pour s'authentifier.
Responder output

![](/assets/images/network_attacks/responder_ntlm_hash_capture.png)

## 5. DHCP Starvation & Rogue DHCP Server
Le DHCP Starvation consiste à épuiser le pool d'adresses IP disponibles du serveur DHCP légitime en envoyant un grand nombre de requêtes avec des adresses MAC différentes. Une fois le pool épuisé (ou en parallèle), l'attaquant peut déployer un Rogue DHCP Server qui répond plus vite que le serveur légitime et impose sa propre passerelle ainsi que ses propres serveurs DNS aux nouveaux clients : c'est une forme de MITM qui ne nécessite aucun empoisonnement ARP.

## 6. VLAN Hopping
Le VLAN Hopping permet à un attaquant de faire passer son trafic d'un VLAN vers un autre sans transiter par un routeur. Deux techniques principales : le switch spoofing (l'attaquant imite un switch et négocie un lien trunk avec DTP) et le double tagging (une trame porte deux en-têtes 802.1Q imbriqués ; le premier switch retire le tag externe et transmet la trame, avec le tag interne, vers un autre VLAN).

## 7. Attaques STP (Spanning Tree Protocol)
STP évite les boucles réseau en élisant un « root bridge ». Un attaquant peut envoyer des BPDU (Bridge Protocol Data Unit) annonçant une priorité très basse pour se faire élire root bridge : tout le trafic inter-switch est alors recalculé pour transiter par lui, ce qui facilite l'interception et peut provoquer des interruptions de service pendant la reconvergence.
Dans notre scénario, nous allons utiliser un switch en guise de « PC du hacker » pour simuler l'attaque, mais il existe aussi des programmes comme Yersinia qui permettent de lancer ce type d'attaques réseau.
Nous pouvons voir que, pour le VLAN 1, le root bridge est le switch Switch1, ce que confirment les informations affichées à côté : Spanning tree root before attack

![](/assets/images/network_attacks/spanning_tree_root_before.png)

Juste après avoir branché notre switch malveillant et propagé son bridge ID sur le réseau, celui-ci est élu root bridge : tout le trafic du réseau passe désormais par lui. Spanning tree root afte attack

![](/assets/images/network_attacks/spanning_tree_afte.png)

## 8. CDP / LLDP Spoofing

CDP (Cisco Discovery Protocol) et LLDP (Link Layer Discovery Protocol) diffusent en clair des informations sur les équipements (modèle, version d'OS, VLAN, IP de gestion...). Un attaquant en écoute passive obtient ainsi une cartographie précise du réseau ; il peut aussi injecter de fausses trames CDP/LLDP pour perturber la découverte de topologie ou saturer la mémoire de certains équipements.

Ici, dans notre cas, nous sommes parvenus, à partir de Switch1, à collecter des informations sur d'autres switches (ce qui est un comportement normal, puisque c'est à cela que sert ce protocole). Mais dans un cas réel, étant donné que nous ne pouvons pas nous promener avec un switch sous le bras, nous pouvons lancer des programmes qui simulent un switch pour récolter ou injecter de fausses informations.
![cdp output : show cdp entry *](/assets/images/network_attacks/cdp_information.png)

## 9. NDP Spoofing & Rogue RA (IPv6)
En IPv6, le protocole NDP (Neighbor Discovery Protocol) remplace ARP et souffre des mêmes faiblesses : l'absence d'authentification. Un attaquant peut falsifier des messages Neighbor Advertisement (l'équivalent de l'ARP spoofing) ou diffuser de faux Router Advertisement (RA) pour se faire passer pour la passerelle IPv6 par défaut. Cela fonctionne même sur des réseaux où l'IPv6 n'est pas officiellement déployé, tant qu'il n'est pas désactivé sur les postes.

## 10. Session Hijacking & SSL Stripping

![](https://cheapsslweb.com/blog/wp-content/uploads/2025/09/ssl-hijacking-attack.webp)

Une fois le trafic intercepté (via ARP spoofing par exemple), l'attaquant peut voler un cookie ou un token de session (Session Hijacking) pour usurper une session déjà authentifiée. Le SSL Stripping consiste à intercepter la première requête HTTP d'une victime, avant qu'elle ne soit redirigée vers HTTPS, et à lui servir une version HTTP tout en maintenant, côté attaquant, une connexion HTTPS avec le vrai site. La victime croit être en clair avec le site légitime, sans que son navigateur n'affiche d'alerte flagrante.

## 11. Déni de service local (SYN / ICMP / IGMP flood)
Sur un segment local, un attaquant peut aussi viser la disponibilité plutôt que la confidentialité : SYN flood (épuisement des files d'attente de connexion TCP), ICMP flood (saturation de bande passante ou de CPU), ou abus de protocoles de diffusion de groupe (IGMP) pour perturber le fonctionnement des switches gérant le multicast.
De cette manière, nous pouvons rendre un service du réseau indisponible, et ainsi empêcher les utilisateurs légitimes d'en tirer profit.
Wireshark DOS Attack monitoring

![](/assets/images/network_attacks/wireshark_dos_attack.png)

## Conclusion
Comme nous venons de le voir, la plupart de ces attaques n'exploitent pas une faille logicielle au sens strict, mais l'absence native d'authentification ou de vérification dans des protocoles conçus à une époque où la confiance entre machines d'un même réseau local n'était pas remise en question (ARP, DNS, DHCP, STP, CDP/LLDP, NDP...). C'est précisément ce qui les rend aussi simples à mettre en œuvre qu'efficaces une fois un attaquant positionné sur le LAN.

Côté défense, quelques réflexes limitent fortement ces risques : activer la sécurité des ports et le DHCP Snooping / Dynamic ARP Inspection sur les switches, verrouiller le VLAN natif et désactiver les trunks automatiques là où ils ne sont pas nécessaires, protéger l'élection du root bridge avec BPDU Guard / Root Guard, désactiver CDP/LLDP sur les ports qui n'en ont pas besoin, et privilégier des protocoles authentifiés (HTTPS partout, DNS over TLS/HTTPS quand c'est possible). Aucune de ces mesures n'est infaillible seule, mais combinées, elles réduisent considérablement la surface qu'un attaquant peut exploiter simplement en étant sur le même réseau que nous.