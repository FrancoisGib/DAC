# DAC TP3

### 01

On peut override le path et faire en sorte que ce ne soit pas echo qui soit exécuter mais un autre fichier.

Le problème vient de la command echo qui se trouvent au /bin/echo,
il suffit simplement de changer le path pour ajouter un directory avec une autre commande echo qui pourrait être n'importe quoi, par exemple on aurait un fichier echo dans le directory /home/flag01, avec les permissions d'éxécuter le fichier pour chacun des utilisateurs de la machine.

On remplace le PATH par /home/flag01:$PATH

```c
  system("/usr/bin/env echo and now what?");
```

Pour fixer la faille, il faudrait plutot exécuter cette ligne.

```c
  system("/usr/bin/env /bin/echo and now what?");
```

Pour récupérer le flag, on créé un fichier echo dans /tmp par exemple avec comme contenu `getflag`, on le rend exécutable avec `chmod +x /tmp/echo`, on remplace le PATH par /tmp:$PATH et on peut récupérer le flag en lançant le fichier ./flag01

### 02

On peut changer la variable d'environnement USER en une commande bash et donc exécuter n'importe quel fichier.

Par exemple:

```bash
export USER=$(ps)
./flag02
```

Pour récupérer le flag, on fait:

```bash
USER='$(getflag)' ./flag02
```

Et on peut remplacer getflag par n'importe quelle commande.

### 03

Le fichier writable.sh est un script qui exécute les fichiers présents dans le répertoire writable.d puis les supprime, et ce toute les 5 minutes.

### 04

Ici, le problème est qu'on a pas les permissions de lire le fichier token.
Par contre, grâce au programme flag04, on a les permissions d'utilier le suid (SUID: Set User ID), qui est ici flag04, ce qui veut dire que, quand on éxécute le programme, on l'éxécute avec l'utilisateur **flag04**.

```
-rwsr-x--- 1 flag04 level04 7428 2011-11-20 21:52 flag04
```

Cependant, cela ne suffit pas, car le fichier à ouvrir est le fichier token. Il faut qu'on fasse passer ce fichier pour un autre en créant un lien symbolique appelé différemment de token vers ce fichier, le programme ne vérifiant pas que le fichier ouvert n'est pas token.

```bash
ln -s /home/flag04/token /tmp/file.txt
```

```bash
./flag04 /tmp/file.txt
```

On récupère ensuite le token et on peut se login avec l'utilisateur flag04 et récupérer le drapeau.

### 05

On doit trouver des répertoires vulnerables et essayer de trouver des informations sensibles.
La plupart des fichiers et dossiers sont inaccessibles mais on remarque que le dossier **.backup** possède un sticky bit d'éxécution (x), ce qui veut dire qu'on peut utiliser un éxécutable pour récupérer les données du répertoire.
On peut notamment utiliser ls pour lister ou cat pour afficher les données du répertoire.
On constate qu'on a une archive à l'intérieur avec la permission de lecture du sticky bit. On peut donc copier le contenu dans un répertoire où on va pouvoir la décompresser.
```bash
cp ./.backup/backup-19072011.tgz ../level05/
```

On peut ensuite décompresser l'archive et on récupère la clé ssh privée de l'utilisateur. Et on peut se connecter en ssh et récupérer le flag.

### 06

On nous dit que les credentials de l'utilisateur viennent d'un os legacy unix (legacy = système dépassé mais encore utilisé).
Les mots de passes sont alors stockés avec un hash dans le fichier /etc/passwd, on récupère donc le hash de l'utilisateur flag06, ce qui donne :
*flag06:ueqwOCnSGdsuM:993:993::/home/flag06:/bin/sh*. On peut ensuite le cracker avec un outil de crackage de mot de passe, ici j'utilise John the reaper.
On fait donc `john password` et on récupère le mot de passe "*hello*".
On peut donc maintenant se connecter en ssh à l'utilisateur flag06 avec son mot de passe et on récupère le flag.