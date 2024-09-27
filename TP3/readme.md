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

Cela va exécuter la commande ps.

### 3

Le fichier writable.sh est un script qui exécute les fichiers présents dans le répertoire writable.d puis les supprime, et ce toute les 5 minutes.

### 4

Ici, le problème est qu'on a pas les permissions de lire le fichier token.
Par contre, grâce au programme flag04, on a les permissions de changer le user id (SUID: Set User ID)

```
-rwsr-x--- 1 flag04 level04 7428 2011-11-20 21:52 flag04
```

Grâce au s de -rwsr, on peut utiliser une commande qui dispose des droits administrateurs pour changer le user id et pouvoir lire le token.
On peut notamment utiliser xxd qui dispose des autorisations root et donc permet de lire le fichier token.

bash

```bash
./flag04 token | xxd | getflag
```

ou

```bash
cat token | xxd | getflag
```
