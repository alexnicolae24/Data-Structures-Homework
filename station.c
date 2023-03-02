#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "station.h"


/* Creeaza o gara cu un numar fix de peroane.
 * 
 * platforms_no: numarul de peroane ale garii
 *
 * return: gara creata
 */
TrainStation* open_train_station(int platforms_no) {
    TrainStation *trainStation = (TrainStation *) calloc(1, sizeof(TrainStation)); // crearea si alocarea unei gari
    if (trainStation == NULL) {
        return NULL;
    }
    trainStation->platforms_no = platforms_no; // setarea numarului de peroane
    trainStation->platforms = (Train **) calloc(platforms_no, sizeof(Train *)); // alocarea vectorului de peroane
    if (trainStation->platforms == NULL) {
        free(trainStation);
        return NULL;
    }
    return trainStation;
}


/* Elibereaza memoria alocata pentru gara.
 *
 * station: gara existenta
 */
void close_train_station(TrainStation *station) {
    if (station == NULL) {
        return;
    }
    for (int i = 0; i < station->platforms_no; i++) {
        // elibereaza memoria pentru fiecare vagon
        if (station->platforms[i] != NULL) { 
            TrainCar *trainCar = station->platforms[i]->train_cars;
            while (trainCar != NULL) {
                TrainCar *tNext = trainCar->next;
                free(trainCar);
                trainCar = tNext;
            }
        }
        free(station->platforms[i]); //elibereaza meoria pentru peron
    }
    free(station->platforms); // elibereaza memoria pentru vectorul de peroane
    free(station); // elibereaza memoria pentru gara
}


/* Afiseaza trenurile stationate in gara.
 *
 * station: gara existenta
 * f: fisierul in care se face afisarea
 */
void show_existing_trains(TrainStation *station, FILE *f) {
    if (station == NULL) {
        return ;
    }
    for (int i = 0; i < station->platforms_no; i++) {
        fprintf(f, "%d: ", i); // afisarea numarului fiecarui peron
        if (station->platforms[i] != NULL) {
            fprintf(f, "(%d)", station->platforms[i]->locomotive_power); // afisarea puterii locomotivei
            TrainCar *trainCar = station->platforms[i]->train_cars;
            while (trainCar != NULL) { // parcurgerea si afisarea greutatii fiecarui vagon
                fprintf(f, "-|%d|", trainCar->weight);
                trainCar = trainCar->next;
            }
        }
        fprintf(f, "\n");
    }
}


/* Adauga o locomotiva pe un peron.
 * 
 * station: gara existenta
 * platform: peronul pe care se adauga locomotiva
 * locomotive_power: puterea de tractiune a locomotivei
 */
void arrive_train(TrainStation *station, int platform, int locomotive_power) {
    //verificarea cazurilor limita
    if (station == NULL) {
        return;
    }

    if (platform < 0 || platform >= station->platforms_no) {
        return ;
    }
    
    //adaugarea locomotivei pe peron
    Train *train = station->platforms[platform];
    if (train == NULL) { 
        station->platforms[platform] = (Train *) calloc (1, sizeof(Train));// alocare memorie pentru locomotiva
        station->platforms[platform]->locomotive_power = locomotive_power;
        station->platforms[platform]->train_cars = NULL;
    }
}


/* Elibereaza un peron.
 * 
 * station: gara existenta
 * platform: peronul de pe care pleaca trenul
 */
void leave_train(TrainStation *station, int platform) {
    //verificarea cazurilor limita
    if (station == NULL) {
        return ;
    }
    if (platform < 0 || platform >= station->platforms_no) {
        return ;
    }
    if (station->platforms[platform] != NULL) {
        if (station->platforms[platform]->train_cars != NULL) {
            //eliberarea memoriei tuturor vagoanelor legate de locomotiva de pe peron
            TrainCar *trainCar = station->platforms[platform]->train_cars;
            while (trainCar != NULL) {
                TrainCar *tNext = trainCar->next;
                free(trainCar);
                trainCar = tNext;
            }
        }
    }
    free(station->platforms[platform]); // eliberarea memoriei locomotivei
    station->platforms[platform] = NULL;
}


/* Adauga un vagon la capatul unui tren.
 * 
 * station: gara existenta
 * platform: peronul pe care se afla trenul
 * weight: greutatea vagonului adaugat
 */
void add_train_car(TrainStation *station, int platform, int weight) {
    //verificarea cazurilor limita
    if (station == NULL) {
        return;
    }
    if (platform < 0 || platform >= station->platforms_no) {
        return ;
    }
    if (station->platforms[platform] == NULL) {
        return ;
    }

    if (station->platforms[platform]->train_cars == NULL) {
        // cazul in care niciun vagon nu era legat de tren
        station->platforms[platform]->train_cars = (TrainCar *) calloc(1, sizeof(TrainCar));
        if (station->platforms[platform]->train_cars == NULL) {
            return ;
        }
        station->platforms[platform]->train_cars->weight = weight;
        station->platforms[platform]->train_cars->next = NULL;
    } 
    else {
        // parcurgerea vectorului de vagoane
        TrainCar *t = (TrainCar *) calloc(1, sizeof(TrainCar));
        if (t == NULL) {
            return;
        }
        t->weight = weight;
        t->next = NULL;
        TrainCar *p = station->platforms[platform]->train_cars;
        while (p->next != NULL) {
            p = p->next;
        }
        p->next = t; // adaugarea vagonului la finalul vectorului
    }
}


/* Scoate vagoanele de o anumita greutate dintr-un tren.
 * 
 * station: gara existenta
 * platform: peronul pe care se afla trenul
 * weight: greutatea vagonului scos
 */
void remove_train_cars(TrainStation *station, int platform, int weight) {
    // verificarea cazurilor limita
    if (station == NULL) {
        return;
    }
    if (platform < 0 || platform >= station->platforms_no) {
        return ;
    }
    if (station->platforms[platform] == NULL) {
        return ;
    }
    if (station->platforms[platform]->train_cars == NULL) {
        return ;
    }

    TrainCar *prev = NULL;
    TrainCar *t = station->platforms[platform]->train_cars;

    // parcurgerea vectorului de vagoane
    while (t != NULL) {
        if (t->weight == weight) { // verificare daca vagonul trebuie eliminat
            if (prev == NULL) { // eliminarea in cazul in care primul vagon trebuie eliminat
                station->platforms[platform]->train_cars = t->next;
                free(t);
                t = station->platforms[platform]->train_cars;
            } else { // eliminarea in cazul in care vagonul nu este primul
                prev->next = t->next;
                free(t);
                t = prev->next;
            }
        } else { // vagonul nu trebuie eliminat
            prev = t;
            t = t->next;
        }
    }
}


/* Muta o secventa de vagoane dintr-un tren in altul.
 * 
 * station: gara existenta
 * platform_a: peronul pe care se afla trenul de unde se scot vagoanele
 * pos_a: pozitia primului vagon din secventa
 * cars_no: lungimea secventei
 * platform_b: peronul pe care se afla trenul unde se adauga vagoanele
 * pos_b: pozitia unde se adauga secventa de vagoane
 */
void move_train_cars(TrainStation *station, int platform_a, int pos_a, 
                                int cars_no, int platform_b, int pos_b) {
    //verificarea cazurilor limita
    if (station == NULL) {
        return ;
    }
    if (cars_no == 0) {
        return ;
    }
    if (platform_a < 0 || platform_a >= station->platforms_no) {
        return ;
    }
    if (platform_b < 0 || platform_b >= station->platforms_no){
        return ;
    }
    if (station->platforms[platform_a]->train_cars == NULL) {
        return ;
    }

    int size_a = 0;
    int size_b = 0;

    // numararea vagoanelor de pe peronul B
    TrainCar *tb = station->platforms[platform_b]->train_cars;
    while (tb != NULL) {
        tb = tb->next;
        size_b++;
    }

    //  numararea vagoanelor de pe peronul A
    TrainCar *ta = station->platforms[platform_a]->train_cars;
    while (ta != NULL) {
        ta = ta->next;
        size_a++;
    }

    // verificarea parametrilor de pozitie si numarului de vagoane de mutat
    if (pos_a < 1 || pos_a > size_a || pos_a - 1 + cars_no > size_a) {
        return ;
    }

    if (pos_b < 1 || pos_b > size_b + 1) {
        return ;
    }
    
    TrainCar *t = station->platforms[platform_a]->train_cars;
    TrainCar *t_a_first_prev = NULL;
    TrainCar *t_a_first = NULL;
    TrainCar *t_a_last = NULL;

    // calcularea vagonului de inceput din secventa de mutat
    int pos = 1;
    while (pos < pos_a && t != NULL) {
        t_a_first_prev = t;
        t = t->next;
        pos++;
    }
    t_a_first = t;

    // calcularea vagonului de final de mutat
    int count = 1;
    while (count < cars_no && t != NULL) {
        t = t->next;
        count++;
    }
    t_a_last = t;

    // stergerea legaturilor de pe peronul a
    if (t_a_first_prev == NULL) {
        station->platforms[platform_a]->train_cars = t_a_last->next;
    } 
    else {
        t_a_first_prev->next = t_a_last->next;
    }

    // gasirea si adaugarea pe pozitia corespunzatoare pe peronul B
    pos = 1;
    t = station->platforms[platform_b]->train_cars;
    TrainCar *bprev = NULL;

    while (pos < pos_b && t != NULL) {
        bprev = t;
        t = t->next;
        pos++;
    }

    if (bprev == NULL) {
        station->platforms[platform_b]->train_cars = t_a_first;
        t_a_last->next = t;
    } 
    else {
        bprev->next = t_a_first;
        t_a_last->next = t;
    }
}


/* Gaseste trenul cel mai rapid.
 * 
 * station: gara existenta
 *
 * return: peronul pe care se afla trenul
 */
int find_express_train(TrainStation *station) {
    //verificare caz limita
    if (station == NULL) {
        return 0;
    }

    int maxDiff = 0;
    int peron = 0;
    for (int i = 0; i < station->platforms_no; i++) {
        Train *t = station->platforms[i];
        if (t != NULL){
            if (t->train_cars == NULL) { // doar pentru locomotiva
                if (t->locomotive_power > maxDiff) {
                    maxDiff = t->locomotive_power;
                    peron = i;
                }
            }
            else {
                int sum = 0;
                TrainCar *tCar = t->train_cars;
                while (tCar != NULL) { // suma greutatilor vagoanelor
                    sum += tCar->weight;
                    tCar = tCar->next;
                }
                if (t->locomotive_power - sum > maxDiff) { // calcularea diferentei maxime
                    maxDiff = t->locomotive_power - sum;
                    peron = i;
                }
            }
        }
    }
    return peron;
}


/* Gaseste trenul supraincarcat.
 * 
 * station: gara existenta
 *
 * return: peronul pe care se afla trenul
 */
int find_overload_train(TrainStation *station) {
    //verificarea cazului de exceptie
    if (station == NULL) {
        return -1;
    }

    for (int i = 0; i < station->platforms_no; i++) {
        Train *t = station->platforms[i];
        if (t != NULL){
            if (t->train_cars != NULL) {
                int sum = 0;
                TrainCar *tCar = t->train_cars;
                while (tCar != NULL) { // calculul sumei greutatii vagoanelor
                    sum += tCar->weight;
                    tCar = tCar->next;
                }
                if (sum > t->locomotive_power) { // daca greutatea totala a vagoanelor > puterea locomotivei => overload
                    return i;
                }
            }
        }
    }
    return -1;
}


/* Gaseste trenul cu incarcatura optima.
 * 
 * station: gara existenta
 *
 * return: peronul pe care se afla trenul
 */
int find_optimal_train(TrainStation *station) {
    //verificarea cazului de exceptie
    if (station == NULL) {
        return 0;
    }

    int minDiff = 0x7FFFFFFF;
    int peron = 0;
    for (int i = 0; i < station->platforms_no; i++) {
        Train *t = station->platforms[i];
        if (t != NULL){
            if (t->train_cars == NULL) {
                if (t->locomotive_power < minDiff) {
                    minDiff = t->locomotive_power;
                    peron = i;
                }
            } else {
                int sum = 0;
                TrainCar *tCar = t->train_cars;
                while (tCar) {
                    sum += tCar->weight;
                    tCar = tCar->next;
                }
                if (t->locomotive_power - sum < minDiff) {
                    minDiff = t->locomotive_power - sum;
                    peron = i;
                }
            }
        }
    }
    return peron;
}

  
/* Gaseste trenul cu incarcatura nedistribuita bine.
 * 
 * station: gara existenta
 * cars_no: lungimea secventei de vagoane
 *
 * return: peronul pe care se afla trenul
 */

// pentru fiecare peron, se itereaza peste vagoane, si de la fiecare vagon se calculeaza suma greutatilor pt urmatoarele cars_no vagoane, si se gaseste cea mai grea secventa
int find_heaviest_sequence_train(TrainStation *station, int cars_no, TrainCar **start_car) {
    //verificare cazuri limita
    if (station == NULL) {
        *start_car = NULL;
        return -1;
    }
    if (cars_no == 0) {
        *start_car = NULL;
        return -1;
    }

    int maxOverload = 0;
    TrainCar *tMax = NULL;
    int peron = -1;
    for (int i = 0; i < station->platforms_no; i++) {
        if (station->platforms[i] != NULL) {
            Train *t = station->platforms[i];
            if (t->train_cars == NULL) {
                continue;
            }
            TrainCar *tCar = t->train_cars;
            TrainCar *temp = tCar;
            int size = 0;
            while (temp != NULL) {
                temp = temp->next;
                size++;
            }

            if (size < cars_no) {
                continue;
            }
            temp = tCar;

            while (temp) {
                int size = 0;
                int sum = 0;
                TrainCar *temp2 = temp;
                while (temp2 != NULL && size < cars_no) {
                    sum += temp2->weight;
                    temp2 = temp2->next;
                    size++;
                }
                if (size == cars_no) {
                    if (sum > maxOverload) {
                        maxOverload = sum;
                        tMax = temp;
                        peron = i;
                    }
                }
                temp = temp->next;
            }

        }
    }
    *start_car = tMax;
    return peron;
}

// functie de swap intre greutatile a doua noduri
void swap(TrainCar *a, TrainCar *b)
{
    int temp = a->weight;
    a->weight = b->weight;
    b->weight = temp;
}

// metoda de bubble sort a unei liste inlantuite
void sortList(TrainCar *start)
{
    int swapped, i;
    TrainCar *ptr1;
    TrainCar *lptr = NULL;
  
    if (start == NULL)
        return;
  
    do
    {
        swapped = 0;
        ptr1 = start;
  
        while (ptr1->next != lptr)
        {
            if (ptr1->weight < ptr1->next->weight)
            { 
                swap(ptr1, ptr1->next);
                swapped = 1;
            }
            ptr1 = ptr1->next;
        }
        lptr = ptr1;
    }
    while (swapped);
}


/* Ordoneaza vagoanele dintr-un tren in ordinea descrescatoare a greutatilor.
 * 
 * station: gara existenta
 * platform: peronul pe care se afla trenul
 */
void order_train(TrainStation *station, int platform) {
    //verificarea cazurilor de exceptie
    if (station == NULL) {
        return ;
    }
    if (platform < 0 || platform > station->platforms_no) {
        return ;
    }
    if (station->platforms[platform] == NULL) {
        return ;
    } 
    if (station->platforms[platform]->train_cars == NULL) {
        return ;
    }

    //apelarea functiei de sortare pe vectorul de vagoane
    sortList(station->platforms[platform]->train_cars); 
}


/* Scoate un vagon din trenul supraincarcat.
 * 
 * station: gara existenta
 */
void fix_overload_train(TrainStation *station) {
    //verificarea cazului limita
    if (station == NULL) {
        return ;
    }

    int peron = find_overload_train(station); // retinerea pozitiei trenului overload

    //verificarea cazului limita pentru peron
    if (peron == -1) {
        return ;
    }
    if (station->platforms[peron] == NULL) {
        return ;
    }

    TrainCar *t = station->platforms[peron]->train_cars;
    if (t == NULL) {
        return ;
    }

    // calcularea numarului total vagoane
    TrainCar *temp = t;
    int size = 0;
    while (temp != NULL) {
        temp = temp->next;
        size++;
    } 

    //calculul greutatii totale fara un vagon anume
    int *optimalWeights = (int*) calloc (size, sizeof(int)); //in acest vector se retine suma vagoanelor fara unul anume
    for (int i = 0; i < size; i++) {
        temp = t;
        int pos = 0;
        while (temp != NULL) {
            if (pos != i) {
                optimalWeights[i] += temp->weight;
            }
            temp = temp->next;
            pos++;
        }
    }

    // in functie de greutatea totala fara un vagon anume, se calculeaza configuratia optima
    int minWeight = 0x7FFFFFFF;
    int found = -1;
    for (int i = 0; i < size; i++) {
        if (station->platforms[peron]->locomotive_power - optimalWeights[i] >= 0 && station->platforms[peron]->locomotive_power - optimalWeights[i] < minWeight) {
            minWeight = station->platforms[peron]->locomotive_power - optimalWeights[i];
            found = i;
        }
    }

    // dupa ce s-a gasit vagonul ce trebuie scos, acesta se elimina
    if (found != -1) {
        int pos = 0;
        temp = t;
        TrainCar *prev = NULL;
        while (temp != NULL && pos < found) {
            prev = temp;
            temp = temp->next;
            pos++;
        }
        if (pos == found) {
            if (prev == NULL) {
                station->platforms[peron]->train_cars = temp->next;
                free(temp);
            } 
            else {
                prev->next = temp->next;
                free(temp);
            }
        }
    }
    free(optimalWeights);//eliberarea memoriei pentru acest vector
}