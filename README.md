# Projet MNT — génération de raster à partir d’un nuage de points

Ce projet C++ transforme un fichier de points MNT (latitude, longitude, altitude) en une image rasterisée au format **PPM**. Le pipeline applique une projection, une triangulation de Delaunay, une interpolation sur grille puis un rendu colorisé.

## Fonctionnalités

- Lecture d’un fichier texte `lat lon alt`.
- Projection géographique via **PROJ**.
- Triangulation de Delaunay des points projetés.
- Rasterisation sur grille et génération d’une image PPM.
- Option de prétraitement Fourier (désactivée par défaut).

## Prérequis

- Un compilateur C++ (GCC, Clang, etc.)
- CMake ≥ 3.10
- Bibliothèque **PROJ** (`proj`)

Sur Debian/Ubuntu (exemple) :

```bash
sudo apt-get install cmake g++ libproj-dev
```

## Compilation

```bash
mkdir -p build
cmake -S . -B build
cmake --build build
```

L’exécutable généré s’appelle `create_raster`.

## Utilisation

```bash
./build/create_raster <fichier_mnt> <largeur_pixels>
```

Exemple :

```bash
./build/create_raster Guerledan.txt 800
```

Le programme génère un fichier PPM dans le répertoire courant :

- `mnt_sans_fourier.ppm` (par défaut)
- `mnt_avec_fourier.ppm` (si l’option Fourier est activée)

## Format du fichier MNT

Chaque ligne doit contenir trois valeurs numériques séparées par des espaces :

```
<latitude> <longitude> <altitude>
```

Les lignes vides sont ignorées. Toute ligne mal formée provoque une erreur.

## Option de prétraitement Fourier

Le prétraitement Fourier est désactivé par défaut. Pour l’activer, modifiez la variable suivante dans `src/main.cpp` :

```cpp
const bool USE_FOURIER = true;
```

## Structure du projet

- `src/` : implémentation du pipeline (projection, triangulation, rasterisation, etc.).
- `include/` : en-têtes C++.
- `resources/` : palette de couleurs (ex. `haxby.cpt`).
- `tests/` : réservé aux tests (vide pour l’instant).

## Licence

Projet pédagogique — aucune licence explicite fournie.
