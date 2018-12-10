# Database Bridge 
 *Ανάπτυξη Λογισμικού Για Πληροφοριακά Συστήματα  
 Χειμερινό Εξάμηνο 2018 – 2019*  
 - Μπολουδάκης Χαράλαμπος 1115201500103
 - Νιάρχος Γεώργιος 1115201500109

# Περιεχόμενα  
- [Οδηγίες Εκτέλεσης](https://github.com/babisboloudakis/DatabaseBridge#%CE%BF%CE%B4%CE%B7%CE%B3%CE%AF%CE%B5%CF%82-%CE%B5%CE%BA%CF%84%CE%AD%CE%BB%CE%B5%CF%83%CE%B7%CF%82)
- [Hash Functions](https://github.com/babisboloudakis/DatabaseBridge#hash-functions)
- [Tests](https://github.com/babisboloudakis/DatabaseBridge#tests)

# Σύντομη Περιγραφή
Σκοπός του προγράμματος μας είναι η εκτέλεση της πράξης της σύζευξης (join) αλλά και του φίλτρου (filter) πάνω
σε ένα σύνολο από table uint64_t με πολλές στήλες. Αναλυτικότερα, αρχικά φορτώνουμε τα δεδομένα μας στην μνήμη
και υπολογίζουμε κάποια στατιστικά για αυτά. Μόλις τελιώσει αυτή η διαδικασία, αρχίζουμε το parsing και την εκτέλεση
των ερωτημάτων πάνω στα δεδομένα μας. Για την εκτέλεση ενός ερωτήματος, με βάση το ερώτημα και τα στατιστικά
μας κάνουμε κάποιες βελτιστωποιήσεις για την επιτάνχυση της διαδικασίας. Αφού ολοκληρωθούν οι πράξεις, το πρόγραμμα
εκτυπώνει το checksum των ζητούμενων στηλών για έλεγχο των αποτελεσμάτων.

# Βελτιστοποίηση ερωτημάτων
Για την βελτιστοποίηση των ερωτημάτων που γίνονται στα δεδομένα μας, έχουμε εφαρμόσει τα εξής
1) Εκτέλεση πρώτα των φίλτρων από τίς συζεύξεις, ανεξαρτήτως σειράς εισόδου, ώστε να ρίξουμε την διάσταση του προβλήματος
της σύζευξης, που έχει πολύ μεγαλύτερη πολυπλοκότητα από τα φίλτρα
2) Με την βοήθεια διάφορων στατιστικών πάνω στα δεδομένα, αναδιατάσουμε η ακόμα και αγνοούμε πλήρως κάποιες πράξεις χωρίς να
επηρεάζεται το τελικό αποτέλεσμα ( Για παράδειγμα δεν εκτελούμε φίλτρο > 5000 για στήλη που η μέγιστη τιμή είναι 4000 )

# Οδηγίες εκτέλεσης
Παρόλο που δεν έχει νόημα ακόμα η εκτέλεση του προγράμματος, έχουμε φτιάξει μια δικιά μας 
main function στην οποία δημιουργούμε δύο δικές μας σχέσης R, S με hardcoded τρόπο
για να δείξουμε ότι το πρόγραμμα εκτελείτε σωστά. Τα δεδομένα των σχέσεων του παραδοτέου είναι
αυτά της εκφώνησης.
```sh
$ make
$./main
```
ή
```sh
$ make run
```

Υπάρχει επίσης label για clean
```sh
$ make clean
```

# Tests
Το πρόγραμμα δοκιμάστικε και εμφάνισε επιτυχώς τα αποτελέσματα της ζεύξης στις περιπτώσεις κενού join, join με τον ευατό του, τυχαίων δεδομένων, και πολλαπλά ίδιων εμφανιζόμενων δεδομένων στα 2 rel. Το παραδοτέο πρόγραμμα έχει τα δεδομένα της εκφώνησης.  

# Hash functions της Radix Hash Join
**HashFunction1() - H1**  
Στην συνάρτηση κατακερματισμού H1 χωρίζουμε τα δεδομένα με βάση την τιμή των **ν** λιγότερο σημαντικών
bits του ανάλογου payload
> Το **ν** στο πρώτο τμήμα του project επιλέγεται αυθέρετα εφόσον δεν δουλεύουμε ακόμα με την cache
```c++
 int n = 3;
```
**HashFunction2() - H2**  
Στην συνάρτηση κατακερματισμού H2 χωρίζουμε τα δεδομένα με βάση το υπόλοιπο διαίρεσης τους με έναν
πρώτο αριθμό ο οποίος είναι **defined** στην κορυφή του αρχείου. Επιλέξαμε να κάνουμε υπόλοιπο διαίρεσης
με έναν πρώτο αριθμό για να τμηματοπιήσουμε πιο ομαλά τα δεδομένα μας.
> Το **PRIME** στο πρώτο τμήμα του project επιλέγεται αυθέρετα αλλά με τέτοιο τρόπο ώστε η H2 να είναι ανεξάρτητη της H1
```c++
 #define PRIME 7
```


