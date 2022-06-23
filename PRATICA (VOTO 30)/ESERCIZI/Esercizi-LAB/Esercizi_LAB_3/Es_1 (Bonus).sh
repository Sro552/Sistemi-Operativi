# 1. creare un “alias” in bash denominato “feedback” che se invocato attende dall’utente
# un input proponendo il prompt “Come ti chiami?” e rispondendo con “Ciao
# <nome>!” (dove <nome> è l’input immesso) senza però sovrascrivere o impostare
# alcuna nuova variabile nella shell attiva.


alias feedback='echo Come ti chiami?; read var; echo Ciao $var;'