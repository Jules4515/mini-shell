# MiniShell

Implémentation légère d'un shell Unix en C, avec gestion des pipes, redirections d'E/S, tâches en arrière-plan et chaînage de commandes.

[![Language](https://img.shields.io/badge/language-C-blue.svg)](https://en.wikipedia.org/wiki/C_(programming_language))
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Linux-lightgrey.svg)]()

## Présentation

MiniShell reproduit le comportement fondamental d'un shell Unix. Il parse et exécute des lignes de commande avec le support de :

- **Pipes** (simples et chaînés) : `ls -la | grep .c | wc -l`
- **Exécution en arrière-plan** (`&`) : `sleep 10 &`
- **Redirection de stdout** (`>`) : `echo "hello" > output.txt`
- **Chaînage de commandes** (`;`) : `mkdir test ; cd test ; pwd`
- **Commandes internes (builtins)** : `cd`, `pwd`, `exit`, `jobs`
- **Gestion des signaux** : gestion de `SIGCHLD` pour les processus en arrière-plan

## Compilation

**Prérequis** : GCC, Make, Linux

```bash
# Compiler
make

# Exécuter
./bin/minishell

# Supprimer les fichiers objets
make clean

# Nettoyage complet (objets + binaire)
make aclean

# Recompiler entièrement
make rebuild
```

## Exemples d'utilisation

```bash
# Commandes simples
ls -la
echo "Hello, World!"

# Pipes
cat /etc/hosts | grep localhost
ls -la | grep .c | sort | head -5

# Exécution en arrière-plan
sleep 30 &
jobs                    # Lister les tâches en arrière-plan

# Redirection de la sortie
ps aux > processes.txt

# Chaînage de commandes
ls ; echo "---" ; pwd
compile.sh & echo "Compilation en arrière-plan..."
```

## Architecture

```
minishell/
├── src/
│   ├── main.c          # Point d'entrée — boucle REPL principale
│   ├── shell.c         # Parser et moteur d'exécution des commandes
│   ├── command.c       # Structure Command et gestion des tâches en arrière-plan
│   ├── terminal.c      # Gestion des entrées utilisateur et affichage du prompt
│   └── utils.c         # Fonctions utilitaires pour les chaînes de caractères
├── inc/
│   ├── shell.h
│   ├── command.h
│   ├── terminal.h
│   └── utils.h
├── makefile
├── LICENSE
└── README.md
```

## Fonctionnement

Le shell exécute une boucle Read-Eval-Print (REPL) dans `main.c` :

```
┌─────────────────────────────────────────────────────────┐
│  1. Afficher le prompt (user@host:~/path$)              │
│  2. Lire l'entrée utilisateur                           │
│  3. Valider la syntaxe (rejeter les séquences invalides)│
│  4. Signaler les tâches en arrière-plan terminées       │
│  5. Parser l'entrée en structures Command               │
│  6. Exécuter chaque commande                            │
│  └─→ Retour à l'étape 1                                │
└─────────────────────────────────────────────────────────┘
```

### Stratégie de parsing

Le parser effectue une **tokenisation à trois niveaux** :

1. **Niveau commande** — Découpage par `;` et `&` (marque le flag arrière-plan)
2. **Niveau pipe** — Découpage par `|` et `>` (marque la redirection fichier)
3. **Niveau argument** — Découpage par espaces

Chaque commande est représentée par une structure `Command` :

```c
typedef struct Command {
    char***      args;            // Arguments de chaque sous-commande
    int          n_pipe;          // Nombre de segments de pipe
    int*         n_args;          // Nombre d'arguments par segment
    int          is_file_output;  // Stdout redirigé vers un fichier
    int          is_background;   // Exécution en arrière-plan
    pid_t        pid;             // PID du processus associé
    int          command_id;      // ID de la tâche (affiché entre crochets)
    CommandState state;           // RUNNING, DONE, EXIT_126, EXIT_127
    struct Command *next;         // Pointeur liste chaînée (tâches en arrière-plan)
} Command;
```

### Exécution

- Les **commandes internes** sont exécutées directement dans le processus du shell (pas de fork)
- Les **commandes externes** utilisent `fork()` + `execvp()`
- Les **pipes** sont implémentés avec `pipe()` et `dup2()` pour chaîner stdin/stdout entre processus
- Les **tâches en arrière-plan** sont suivies dans une liste chaînée globale et surveillées via `SIGCHLD`

## Limitations

- Pas de redirection de stderr (`2>`) ni de mode append (`>>`)
- Pas d'expansion de wildcards (`*`, `?`)
- Pas d'expansion de variables d'environnement (`$VAR`)
- Pas de gestion des guillemets pour les arguments avec espaces
- Entrée mono-ligne uniquement (pas de heredoc)

## Licence

Ce projet est sous licence MIT — voir le fichier [LICENSE](LICENSE) pour plus de détails.

## Auteur

Jules Guignard
