# `tsp`

Solveur interactif pour le problème du voyageur de commerce, développé dans le cadre du thème **Mathématiques et mouvement** de la Semaine des mathématiques 2018, au Palais de la Découverte.

## Description

Le problème du voyageur de commerce ([*Travelling Salesman Problem*(en)](http://en.wikipedia.org/wiki/Travelling_salesman_problem)) consiste à trouver le plus court chemin passant par tous les points d'une liste. Il s'agit d'un problème d'optimisation combinatoire réputé difficile, en ce sens que le meilleur algorithme connu qui garantisse de trouver la bonne réponse doit étudier un très grand nombre de trajets, augmentant exponentiellement en fonction du nombre de points à visiter.

Ce programme charge un fichier texte passé en paramètre (ou, à défaut, `france.txt`) depuis le répertoire `assets/`. Ce fichier contient une liste de villes avec leurs coordonnées à l'écran, ainsi qu'une éventuelle image de fond à afficher en guise de carte. Le programme calcule immédiatement le plus court trajet par [force brute(en)](https://en.wikipedia.org/wiki/Brute-force_search), et un trajet "raisonnable" selon la méthode du [plus proche voisin(en)](https://en.wikipedia.org/wiki/Nearest_neighbour_algorithm).

L'utilisateur peut ensuite construire son propre trajet en cliquant sur chacune des villes, et la longueur totale du trajet est affichée en bas de la fenêtre, deux raccourcis permettant de comparer son trajet avec ceux des deux autres algorithmes.

# Déploiement

Le programme fourni est conçu pour un système GNU/Linux ([Debian(fr)](https://www.debian.org/index.fr.html)), mais peut être aisément porté sur d'autres systèmes.

Le fichier `Makefile` fourni permet de compiler le programme C. Plusieurs exemples d'instances sont fournies dans le répertoire `assets/`.

## Paquetages nécessaires (Debian Stretch)
- `gcc`
- `make`
- `libsdl2-dev`
- `libsdl2-image-dev`
- `libsdl2-ttf-dev`

## Usage
```
git clone https://github.com/universcience/tsp
cd tsp
make
./tsp france.txt
```

Le programme affiche sur la sortie standard un trajet optimal et celui obtenu par la méthode du plus proche voisin, ainsi que leurs longueurs respectives. Ces deux trajets peuvent être affichés directement sur la carte en appuyant sur la touche `h` (*Hint*) pour le meilleur trajet ou `n` (*Nearest*) pour les plus proches voisins.

L'utilisateur construit son trajet en cliquant du bouton gauche, dans l'ordre, sur les villes à visiter. Un clic droit permet d'annuler la dernière étape listée. Enfin, le bouton `r` (*Reset*) permet de repartir de zéro en effaçant le trajet en cours.

## Format de fichier

# Licence

## Code source

Ceci est un [logiciel libre(fr)](https://www.gnu.org/philosophy/free-sw.fr.html), distribué sous les termes de la licence GNU GPL Affero. Les termes exacts de cette licence sont disponibles dans le fichier `LICENSE`.

En résumé, vous êtes libres de télécharger ce logiciel, de l'utiliser et de le modifier sans restrictions, ainsi que de le redistribuer sous les mêmes termes, avec ou sans modifications. 

## Images

Les images réunies par commodité dans le dossiers `assets/` le sont à but illustratif, et certaines d'entre elles sont protégées par le copyright. Leur redistribution n'est pas libre de droit, bien qu'elles soient publiquement disponibles sur le web. Pour plus de détails, se reporter au fichier `assets/COPYRIGHT`.
