package main

import (
	"fmt"
	"math/rand"
	"time"
)

const (
	MAXPROC= 10
	MAXBUFF= 10
)

func when(b bool, c chan int) chan int {
	if !b {
		return nil
	}
	return c
}
//-------------------------------------




 

//canali
var done = make(chan bool)
var termina_passerella = make(chan bool)

var ack_viaggiatore[MAXPROC] chan int
var req_viaggiatore_entrata= make(chan int,MAXBUFF) //priorita --> bufferizzato
var req_viaggiatore_uscita= make(chan int,MAXBUFF)

var termina_addetto= make(chan bool)
var req_addetto_pericolo= make(chan int,MAXBUFF) 
var req_addetto_fine_pericolo= make(chan int,MAXBUFF) 
var ack_addetto= make(chan int) 

func viaggiatore(id int) {
	fmt.Printf("viaggiatore %d | avviato\n", id)
	req_viaggiatore_entrata<-id
	<-ack_viaggiatore[id]

	time.Sleep(time.Duration(rand.Intn(5)+2) * time.Second)
	
	req_viaggiatore_uscita<-id
	<-ack_viaggiatore[id]

	done <- true
	fmt.Printf("viaggiatore %d | esce dal sistema\n", id)
}

func addetto() {
	fmt.Printf("addetto @ | avviato\n")

	for  {
		select {
		case <-termina_addetto: //terminaziona addetto  
			fmt.Printf("addetto @ | termina\n")
			done <- true
			return
		default:
			req_addetto_pericolo<-1
			<-ack_addetto
			time.Sleep(time.Duration(rand.Intn(3)+3) * time.Second)
			req_addetto_fine_pericolo<-1
			<-ack_addetto
		}
	}
}

func passerella() {
	fmt.Printf("passerella # | avviato\n")
	var pericolo int=0
	var n_viaggiatori_passerella int=0
	for  {
		//linea di stampa stato
		fmt.Printf("passerella # |\t") 
		fmt.Printf("pericolo: %d\t",pericolo)  
		fmt.Printf("n_viaggiatori_passerella: %d\t",n_viaggiatori_passerella)
		fmt.Printf("\n")

		select {
		case x:= <-when(pericolo==0,req_viaggiatore_entrata):
			//fmt.Printf("viaggiatore_entrata %d entrato \n",x )
			n_viaggiatori_passerella++
			ack_viaggiatore[x] <- 1

		case x:= <-when(true,req_viaggiatore_uscita):
			//fmt.Printf("viaggratore_action %d entrato \n",x )
			n_viaggiatori_passerella--
			ack_viaggiatore[x] <- 1

		case <-when(true,req_addetto_pericolo):
			//fmt.Printf("addetto_pericolo %d entrato \n",x )
			pericolo=1
			ack_addetto <- 1

		case <-when(true,req_addetto_fine_pericolo):
			//fmt.Printf("addetto_fine_pericolo %d entrato \n",x )
			pericolo=0
			ack_addetto <- 1
			
		case <-termina_passerella: //terminazione server
			fmt.Printf("passerella # | termina\n")
			done <- true
			return
		}
	}
}


func main() {

	var nviaggiatori int //numero di client

	fmt.Printf("inserisci nviaggiatori (max %d)? ", MAXPROC)
	fmt.Scanf("%d", &nviaggiatori)

	for i := 0; i < nviaggiatori; i++ { //canali ack per viaggiatore
		ack_viaggiatore[i]= make(chan int,MAXBUFF) 
	}
	go passerella()

	go addetto()


	for i := 0; i < nviaggiatori; i++ {
		go viaggiatore(i)
	}

	//aspetta client
	for i := 0; i < nviaggiatori; i++ {
		<-done
	}

	termina_addetto<- true //terminazione addetto
	<-done

	//aspetta server
	termina_passerella <- true
	<-done
	fmt.Printf("FINE\n")
}

