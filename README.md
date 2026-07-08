# C-CPU
Une implémentation simplifiée d'un CPU en C

# Description:

Ce CPU réalise différentes opérations arithmétiques (ADD, SUB, MULT, INV), opérations logiques (NAND, AND, OR, NOT, XOR, LogicalShift). Les opérations logiques sont construites en utilisant principalement: NAND, RightShift. Les opérations arithmétiques utilisent les opérations logiques, arithmétiques et les indicateurs (l'indicateur CF dans le cadre de MULT). Le CPU contient une ALU et 3 registres pour stocker des valeurs. L'ALU contient un registre accumulateur et 4 indicateurs (ZF,CF,OF,NF).

# Utilisation:

Pour l'utiliser, il faut donc charger une valeur (1.) puis l'envoyervaleur dans le registre accumulateur (2. PASS), charger une deuxième valeur (sans l'envoyer avec PASS) puis demander l'opération.
Le résultat se trouve dans le registre accumulateur (6. pour afficher son contenu).
Pour les opérateurs unaires, il suffit de mettre la valeur dans le registre accumulateur (2. PASS) et demander l'opération.

# Exemple:
--> Quel est votre choix  ? 1
Entrez un nombre :-8
--> Quel est votre choix  ? 2
--> Quel est votre choix  ? 14
apres opp(), A = 0000000000001000
z0c0o0n0
--> Quel est votre choix  ? 6
intValue(accu)=8
--> Quel est votre choix  ? 1
Entrez un nombre :2
--> Quel est votre choix  ? 17
apres mul(), A = 0000000000010000
z0c0o0n0
--> Quel est votre choix  ? 6
intValue(accu)=16
--> Quel est votre choix  ? 1
Entrez un nombre :6
--> Quel est votre choix  ? 9
apres sub(), A = 0000000000001010
z0c1o0n1
--> Quel est votre choix  ? 6
intValue(accu)=10

