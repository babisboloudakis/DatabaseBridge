# Database Bridge 
 *Ανάπτυξη Λογισμικού Για Πληροφοριακά Συστήματα  
 Χειμερινό Εξάμηνο 2018 – 2019*  
 - Μπολουδάκης Χαράλαμπος 1115201500103
 - Νιάρχος Γεώργιος 1115201500109

# Περιεχόμενα  
- [Σύντομη Περιγραφή](https://github.com/babisboloudakis/DatabaseBridge#user-content-σύντομη-περιγραφή)
- [Βελτιστοποίηση ερωτημάτων](https://github.com/babisboloudakis/DatabaseBridge#%CE%B2%CE%B5%CE%BB%CF%84%CE%B9%CF%83%CF%84%CE%BF%CF%80%CE%BF%CE%AF%CE%B7%CF%83%CE%B7-%CE%B5%CF%81%CF%89%CF%84%CE%B7%CE%BC%CE%AC%CF%84%CF%89%CE%BD)
- [Οδηγίες Εκτέλεσης](https://github.com/babisboloudakis/DatabaseBridge#%CE%BF%CE%B4%CE%B7%CE%B3%CE%AF%CE%B5%CF%82-%CE%B5%CE%BA%CF%84%CE%AD%CE%BB%CE%B5%CF%83%CE%B7%CF%82)
- [Tests](https://github.com/babisboloudakis/DatabaseBridge#tests)
- [Hash Functions](https://github.com/babisboloudakis/DatabaseBridge#hash-functions-%CF%84%CE%B7%CF%82-radix-hash-join)

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
2) Με την βοήθεια διάφορων στατιστικών πάνω στα δεδομένα, αναδιατάσουμε την σειρά εκτέλεσης των joins ανάλογα με το πλήθος των στηλών των σχέσεων (λιγότερο μέγεθος πιθανών λιγότερα mathces) ή προβλέποντας το NULL αποτέλεσμα αναδιατάσεται η αντίχτοιχη πράξη στην πρώτη θέση(οι αναδιατάξεις εφαρμόζονται μόνο στην "καλύτερη" πράξη).
3) Με τη χρήση των min, max κατά την εκτέλεση των φίλτρων, προβλέπεται το αποτέσμα αν είναι NULL ή αυτούσιο και η πράξη αυτή γίνεται σε Ο(c) ( Για παράδειγμα δεν εκτελούμε φίλτρο > 5000 για στήλη που η μέγιστη τιμή είναι 4000 )

# Οδηγίες εκτέλεσης
Έχουμε δημιουργήσει Makefile για την μεταγλώτιση και εκτέλεση του προγράμματος.
```sh
$ make
$./main
```
Για άμεση εκτέλεση του προγράμματος
```sh
$ make run
```
Για εκτέλεση του προγράμματος με το small workload
```sh
$ make small
```
Υπάρχει επίσης label για καθαρισμό αντικείμενων αρχείων, και του εκτελέσιμου
```sh
$ make clean
```

# Tests
Το πρόγραμμα ελέγχτηκε με την χρήση των small workloads από
http://sigmod18contest.db.in.tum.de/task.shtml
Σχεδόν όλα τα αποτελέσματα ήταν σωστά εκτός από ελάχιστα τα οποία δουλεύουμε να διορθώσουμε.
Ο χρόνος εκτέλεσης των ερωτημάτων πάνω σε αυτά τα δεδομένα είναι πολύ ικανοποιητικός λόγω των βελτιστοποιήσεων
που έχουν γίνει πάνω στα ερωτήματα καθώς και εξαιτίας της προεπεξεργασίας των δεδομένων.

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


