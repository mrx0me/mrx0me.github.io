---
layout: post
title: WordPress Hacking
image: "/assets/images/web_attacks/wordpress.png"
category: Web Exploitation 
author: mr0me
---

# Avertissement
Ce blog est dédié à la recherche en cybersécurité et à la culture technique. Toutes les informations partagées ici le sont à des fins éducatives, de tests en laboratoire isolé (CTF, machines vulnérables autorisées) et de veille technologique. L'utilisation de ces techniques sur des systèmes sans l'autorisation explicite et écrite du propriétaire est **illégale** et engage la seule responsabilité de l'utilisateur. L'auteur décline toute responsabilité en cas d'usage malveillant ou de dommages causés à des tiers.

# WordPress : L'écosystème le plus attaqué du web

WordPress est l'éditeur de contenu – ou CMS – le plus populaire et le plus utilisé au monde. Créé en 2003 par Matt Mullenweg et Mike Little, l'outil s'est peu à peu imposé au sein des administrations publiques, des marques et des boutiques en ligne, alors qu'il était simplement conçu à l'origine comme un service de blogging. Désormais, pour créer son site web, difficile de passer à côté de WordPress, un outil totalement open source et fort d'une impressionnante communauté de créateurs. 

**Quelques chiffres clés :**
- Plus de **409 millions** de personnes consultent plus de **20 milliards** de pages chaque mois ([source](https://www.blogdumoderateur.com/chiffres-wordpress/))
- WordPress représente environ **43%** de tous les sites web dans le monde
- Près de **60%** des sites utilisant un CMS sont sous WordPress

Cette omniprésence signifie que durant un test de pénétration ou une activité du même type, il y a de fortes chances de tomber sur lui. **Savoir l'exploiter est donc une compétence essentielle à posséder dans son bagage technique.** Dans cet article, nous essaierons de voir les différentes méthodes à suivre pour l'auditer et l'exploiter.

# Phase 1 : L'énumération

Comme pour n'importe quelle attaque envisagée contre un système, il faut toujours commencer par **l'énumération**. Sous WordPress, c'est une étape particulièrement riche et souvent suffisante pour identifier des vecteurs d'attaque.

## Détermination de la version

Pouvoir déterminer la version d'un logiciel utilisé peut très souvent vous faciliter la tâche, car il vous suffirait d'une petite recherche sur Google ou dans des bases de données de CVE (comme le [NIST NVD](https://nvd.nist.gov/)) pour voir s'il existe une vulnérabilité rendue publique qui affecte la cible. Et vous n'auriez besoin que de votre navigateur.

**Exemple concret :** WordPress ≤ 6.2 est vulnérable à une faille de type **Directory Traversal** via le paramètre `wp_lang` (CVE-2023-2745). Cela permet de lire le contenu de n'importe quel fichier sur la machine cible (si le serveur applicatif y a les droits d'accès).

### Méthodes pour identifier la version :

| Méthode | Description | Exemple |
| :--- | :--- | :--- |
| **1. Code source** | Lisez le code source (CTRL+U) de n'importe quelle page du site et cherchez l'expression `wordpress` dans les commentaires ou les métadonnées. | `<meta name="generator" content="WordPress 6.4.3" />` |
| **2. Fichier `readme.html`** | Accédez à `/readme.html`. Ce fichier est souvent présent et affiche clairement la version. | `https://exemple.com/readme.html` |
| **3. Fichier `wp-links-opml.php`** | Ouvrez `/wp-links-opml.php` et la version apparaît dans l'en-tête XML. *(Attention, ce fichier est déprécié depuis WP 5.6)* | `https://exemple.com/wp-links-opml.php` |
| **4. Paramètre `ver=`** | Surveillez le paramètre `ver=` que WordPress ajoute aux URLs des fichiers JavaScript et CSS. | `/wp-includes/js/jquery/jquery.js?ver=3.7.1` |
| **5. En-têtes HTTP** | Analysez les en-têtes HTTP de la réponse. WordPress ajoute parfois `X-Powered-By` ou un en-tête spécifique. | `curl -I https://exemple.com` |


## Énumération des comptes utilisateurs

Par défaut, WordPress divulgue les noms d'utilisateurs des comptes administratifs. Si un attaquant y a accès, cela signifie qu'il ne lui reste plus qu'à trouver le mot de passe correspondant.

### Méthodes pour énumérer les utilisateurs :

1. **Endpoint REST API** : Ouvrez le point de terminaison `/wp-json/wp/v2/users`. WordPress vous indique les noms d'utilisateurs valides (sauf si l'option a été désactivée par un filtre).
   
   ```json
   [
     {
       "id": 1,
       "name": "admin",
       "url": "https://exemple.com",
       "description": "",
       "link": "https://exemple.com/author/admin/",
       "slug": "admin",
       "avatar_urls": {...}
     }
   ]
   ```

2. **Alternative REST** : Si le point de terminaison précédent a été bloqué par un filtre, essayez `/?rest_route=/wp/v2/users`. Cette méthode contourne parfois les restrictions mises en place par des plugins de sécurité ou des règles proxy.

3. **Flux RSS des auteurs** : Utilisez `/feed/` avec le paramètre `?author=ID`. WordPress redirige vers la page de l'auteur si l'ID existe.
   
   ```bash
   curl -I https://exemple.com/?author=1
   # Redirige vers /author/admin/ si l'ID 1 existe
   ```

4. **Sitemap XML** : Explorez `/wp-sitemap.xml` ou `/sitemap.xml` qui listent souvent les auteurs.


# La page de connexion

WordPress met à disposition des administrateurs une interface d'administration. Pour y accéder, il faut passer par la page de connexion qui se trouve toujours à :

- `/wp-login.php`
- `/wp-admin/wp-login.php`
- Ou parfois sur `/admin` (via certains plugins de sécurité)

Vous pouvez utiliser le nom d'utilisateur trouvé précédemment (ou tenter `admin` qui est un nom très commun) et attaquer le mot de passe par **force brute**. 

**Attention :** Les sites modernes implémentent généralement des limites de tentatives de connexion, des CAPTCHA ou une authentification à deux facteurs (2FA). La force brute pure est souvent inefficace sans un contournement supplémentaire.

![Page de connexion WordPress](/assets/images/web_attacks/wp_login.png)

# Service XML-RPC

Le service XML-RPC de WordPress est un parfait exemple de fonctionnalité héritée devenue un **risque de sécurité majeur**. Intégré par défaut depuis la version 3.5, il permettait à des applications externes de communiquer avec le site. Aujourd'hui, il est largement remplacé par l'API REST, plus moderne et sécurisée. Le problème ? **Il reste activé par défaut**, ce qui en fait une cible de choix pour les attaquants.

> **Test simple :** Pour vérifier si le service est actif, envoyez une requête POST sur `/xmlrpc.php` avec le contenu suivant :
> ```xml
> <methodCall><methodName>system.listMethods</methodName></methodCall>
> ```

### Vecteurs d'attaque critiques :

| Vecteur | Description | Impact |
| :--- | :--- | :--- |
| **Force brute amplifiée** | Le XML-RPC permet de regrouper des centaines de tentatives d'identification en une seule requête HTTP via la méthode `system.multicall`. Cela rend les attaques par force brute bien plus rapides et difficiles à détecter par les systèmes de sécurité classiques. | Contournement des limitations de taux |
| **DDoS par amplification** | La méthode `pingback.ping` peut être détournée pour lancer des attaques DDoS. Un attaquant force un site WordPress à envoyer des requêtes vers une cible, utilisant le serveur comme amplificateur de trafic. Cette technique était si problématique qu'elle a été désactivée par défaut dans certains stacks comme Bitnami. | Attaques par déni de service |
| **Fuite d'informations (CVE-2025-54352)** | La vulnérabilité CVE-2025-54352 (affectant WordPress de la version 3.5 à 6.8.2) permet à un attaquant non authentifié de deviner les titres d'articles privés et de brouillons en utilisant la méthode `pingback.ping`. | Exposition de données confidentielles |
| **SSRF (Server-Side Request Forgery)** | La méthode `pingback.ping` peut être exploitée pour une attaque de type SSRF. Le serveur, sans validation adéquate, peut être manipulé pour envoyer des requêtes vers des services internes (serveurs de base de données, API métriques, etc.), exposant ainsi l'infrastructure réseau interne. | Accès à l'infrastructure interne |
| **Contournement de protections** | La même méthode est utilisée pour contourner des protections comme Cloudflare et révéler l'adresse IP réelle du serveur. | Découverte de l'infrastructure |

### Outils disponibles :

De nombreux outils sur [GitHub](https://www.google.com/search?q=site%3Agithub.com+xmlrpc+exploit) existent pour exploiter ces vecteurs. Parmi les plus connus :
- **xmlrpc-bruteforce.py** : Force brute optimisée via `system.multicall`
- **WordPress Pingback DDoS Tool** : Exploitation de l'amplification
- **wpxmlrpc** : Suite d'outils pour tester les vulnérabilités du XML-RPC


# Mauvaises configurations

Parmi les erreurs courantes conduisant à la prise de contrôle d'un site WordPress, nous pouvons citer :

### 1. Enregistrement des comptes activé
Parfois, l'enregistrement des comptes dans l'administration est activé (paramètre `membership`). Vous pouvez créer un compte, souvent avec des privilèges limités, en accédant à :
```
/wp-login.php?action=register
```
Une fois connecté, une escalade de privilèges peut être tentée via d'autres vulnérabilités.

### 2. Anciennes versions de fichiers sensibles
Des fichiers sensibles comme `wp-settings.php` ou `wp-config.php` peuvent être accessibles avec des suffixes mal protégés :
- `.old`, `.backup`, `.bak`
- `.1`, `.2`, `.3` (versions antérieures)
- `~` (fichier temporaire de sauvegarde)

### 3. Indexation des répertoires activée
Si l'indexation des répertoires est activée (option `Options +Indexes` dans Apache), un attaquant peut explorer le contenu des dossiers :
- `/wp-content/uploads/` (peut contenir des fichiers exécutables, des shells, etc.)
- `/wp-content/plugins/`
- `/wp-content/themes/`

### 4. Fichiers de sauvegarde de base de données
Certains administrateurs créent des sauvegardes SQL dans les répertoires publics :
- `/backup.sql`
- `/db-backup-2024.sql`
- `/wordpress.sql`

### 5. Debug mode activé
Si `WP_DEBUG` est activé dans `wp-config.php`, le site peut afficher des informations sensibles sur l'infrastructure, les chemins de fichiers et les requêtes SQL.

---

# Les Eextensions (Plugins & Thèmes)

Sous WordPress, il est possible d'utiliser des **extensions** (plugins) et des **thèmes** pour ajouter des fonctionnalités ou personnaliser l'apparence. La plupart des extensions ne sont pas gérées par WordPress core, ce qui signifie que la qualité du code et les mécanismes de sécurité dépendent entièrement des développeurs tiers.

**Point critique :** Comme pour WordPress lui-même, si vous pouvez déterminer la version des extensions utilisées, vous pouvez chercher des failles rendues publiques qui les concernent.

### Méthodes pour identifier les extensions :

| Méthode | Détails |
| :--- | :--- |
| **1. Code source** | Recherchez les termes `plugin` et `theme` dans le code source. Les URLs des assets (CSS, JS) contiennent souvent le nom du plugin. |
| **2. Outils de fingerprinting** | Utilisez des extensions de navigateur comme **Wappalyzer** (disponible sur Chrome et Firefox) ou **BuiltWith** qui listent les technologies utilisées sur un site. |
| **3. Fichiers statiques** | Essayez d'accéder à des fichiers comme `/wp-content/plugins/[nom-plugin]/readme.txt` ou `/wp-content/plugins/[nom-plugin]/changelog.txt` qui révèlent souvent la version. |
| **4. En-têtes HTTP** | Certains plugins ajoutent des en-têtes HTTP spécifiques avec leur nom et leur version. |
| **5. Commentaires HTML** | Les plugins laissent parfois des commentaires dans le code HTML indiquant leur présence. |

### Bases de données de vulnérabilités :
- **WPScan Vulnerability Database** : La référence (plus de 21 000 failles recensées)
- **Exploit-DB** : Exploits publics
- **CVE MITRE** : Base de données officielle
- **Patchstack** : Vulnérabilités des plugins WordPress


# Outil automatisé : WPScan

Toutes les recherches manuelles que nous avons effectuées ci-dessus peuvent être automatisées avec un outil populaire : **[WPScan](https://github.com/wpscanteam/wpscan/)**.

WPScan est un outil d'analyse de sécurité **black box** spécialisé pour WordPress, écrit en Ruby et disponible gratuitement pour un usage non commercial. Lancé en 2011, il est devenu un standard pour les professionnels de la sécurité et les administrateurs de sites.

> **Important :** WPScan nécessite un **jeton API** pour accéder à la base de données des vulnérabilités (WPVulnDB). Un plan gratuit existe avec 50 requêtes par jour. Sans jeton, WPScan énumère les composants mais **ne détecte pas les vulnérabilités**.

### Exemples concrets d'utilisation

| Commande | Explication |
| :--- | :--- |
| `wpscan --url https://exemple.com` | Effectue un scan de base (version WordPress, thème actif, sécurités passives) |
| `wpscan --url https://exemple.com -e vp` | Énumère uniquement les plugins vulnérables (mode "vulnerable plugins") |
| `wpscan --url https://exemple.com --enumerate u` | Énumère la liste des utilisateurs du site |
| `wpscan --url https://exemple.com -P wordlist.txt` | Lance une attaque par force brute sur tous les comptes avec une liste de mots de passe |
| `wpscan --url https://exemple.com -e u --passwords /chemin/wordlist.txt --usernames admin` | Lance une attaque par force brute ciblée sur le compte "admin" |
| `wpscan --url https://exemple.com --api-token TOKEN` | Utilise votre jeton API pour la détection des vulnérabilités |
| `wpscan --url https://exemple.com --random-user-agent` | Utilise un User-Agent aléatoire pour éviter la détection |
| `wpscan --url https://exemple.com --enumerate t` | Énumère les thèmes installés |

### Caractéristiques avancées :

- **Énumération passive et agressive** : Deux modes pour l'énumération des utilisateurs et des composants
- **Détection des failles de sécurité** : Plus de 21 000 vulnérabilités référencées dans WPVulnDB
- **Attaque par dictionnaire** : Support de multiples formats de wordlists
- **Reprise d'attaque** : Possibilité de reprendre une attaque interrompue
- **Export des résultats** : Formats JSON, CSV et rapport HTML


# Conclusion

L'audit de sécurité d'un site WordPress repose sur une méthodologie rigoureuse combinant :

1. **Reconnaissance passive** (code source, fichiers publics)
2. **Énumération active** (API REST, XML-RPC, endpoints)
3. **Identification des composants** (version core, plugins, thèmes)
4. **Automatisation** avec des outils comme WPScan
5. **Exploitation ciblée** des vulnérabilités identifiées

La popularité de WordPress en fait une cible privilégiée, mais elle offre aussi une richesse d'information pour les auditeurs. **La clé est de maîtriser la phase d'énumération** : plus vous en savez sur la cible, plus vos chances de succès sont élevées.

