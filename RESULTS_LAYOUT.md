Rezulati pokretanja
===================

Na svim pokretanjima dobijem ocekivane rezultate. Na vecem primjeru, dobijem
manje kontigova (43 naprema 47), medjutim to je i za ocekivati buduci da
nesto agresivnije racunam tranzitivne bridove. Nisam nikako uspio pogoditi koje
parametre oni koriste u tim slucajevima. Na malom primjeru, dobijem potpuno
isto rjesenje kao i minimus.

Najsporiji dio u rjesenju je IO - na to se potrosi 90% vremena, prema
callgrindu. Sve 3 faze unitigginga otprilike su podjednako zastupljene. 

Kada spojimo rjesenje, problem s IO ce nestati, tako da ce to biti
najozbiljnija optimizacija. 

Mali primjer
------------

Prvih 500 redaka iz readsim-1.5/example/ecoli/NC_000913.fna, sto je ukupno
34390 baza. 

* 167 readova
* 1323 overlapa

Minimus (faza layouta):   0 s

main_layout (unitigging): 0.00 s

main_layout (kompletan):  0.03 s 

Veliki primjer
--------------

Cijela readsim-1.5/example/ecoli/NC_000913.fna, sto je ukupno 4638675 baza.

* 23856 readova
* 249674 overlapa

Minimus (faza layouta):   2 s

main_layout (unitigging): 0.09 s

main_layout (kompeltan):  3.53 s


