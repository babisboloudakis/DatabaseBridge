# Database Bridge 
 *Ανάπτυξη Λογισμικού Για Πληροφοριακά Συστήματα,
 Χειμερινό Εξάμηνο 2018 – 2019*

Οδηγίες εκτέλεσης για demonstration
```
$ make
$./main
```
ή
```
$ make run
```

Υπάρχει επίσης label για clean
```
$ make clean
```

# Hash functions
1. H1
> Στην συνάρτηση κατακερματισμού H1 χωρίζουμε τα δεδομένα με βάση την τιμή των **ν** λιγότερο σημαντικών
bits του ανάλογου payload
2. H2
> Στην συνάρτηση κατακερματισμού H2 χωρίζουμε τα δεδομένα με βάση το υπόλοιπο διαίρεσης τους με έναν
πρώτο αριθμό ο οποίος είναι **defined** στην κορυφή του αρχείου. Επιλέξαμε να κάνουμε υπόλοιπο διαίρεσης
με έναν πρώτο αριθμό για να τμηματοπιήσουμε πιο ομαλά τα δεδομένα μας.

# Αποτελέσματα Ζεύξης
Τα αποτελέσματα της ζεύξης αποθηκεύονται σε μια δικιά μας δομή δεδομένων με μορφή λίστας.
Η δομή που αντιστοιχεί στον κώδικα είναι η
```
class ResultList {
// Implementation
}
```
Η ResultList αποθηκέυει σε μορφή λίστας μια σειρά από buckets τα οποία περιέχουν
* 1ΜΒ buffer
* δείκτη στο επόμενο bucket
