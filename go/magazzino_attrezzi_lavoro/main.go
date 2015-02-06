//negozianti: acq almeno K esemplari
//privati: 1 articolo alla volta
//NB: 1 solo tipo di prodotto
//fornitori: periodicamente consegna 1 lotto di MAX esemplare di 1 solo articolo

//3task: magazzino,clienti,fornitori
//privilegiare negozianti risp privati

//priorita fornitori: martelli,tenaglie,badili

//var req_tenaglie= make(chan int ,MB)	//canale per le richieste dei fornitori
//var req_badile= make(chan int ,MB)	//canale per le richieste dei fornitori

// Package main provides ...
package main

import (
	"fmt"
	"math/rand"
	"time"
)

const (
	MAX int = 2 //max esemplare in mag per mart,ten,badile
	MAX_CLIENTI int = 1 
	MAX_FORNITORI int = 1 
	K int = 1
	MB int = 1

	//tipologie articolo
	martello=0
	tenaglia=1
	badile=2
)

type richiesta_negoziante struct {
	id int
	qta int
	tipo_articolo int
}

type consegna_fornitore struct {
	id int
	qta int
	tipo_articolo int
}

//due canali per cliente e negoziante, per ognuno martello,tenaglia,badile
var req_negoziante[3] chan richiesta_negoziante//1 canale per ogni articolo
var req_privato[3] chan int//1 canale per ogni articolo
var res_cliente[MAX_CLIENTI] chan int	//canale di risposta all'attesa indipendentemente dal tipo

var req_fornitore[3] chan consegna_fornitore	//1 canale per ogni articolo
var res_fornitore[MAX_FORNITORI] chan int	//canale di risposta all'attesa indipendentemente dal tipo

var done = make(chan bool )
var termina = make(chan bool)

func when(b bool, c chan int) {
	if !b {
		return nil
	}
	return c
}

func new_req_negoziante(id int) (richiesta_negoziante){
	var rng richiesta_negoziante
	rng.id=id
	rng.qta=rand.Intn(3) + K
	rng.tipo_articolo = rand.Intn(3)
	return rng
}

func new_cons_fornitore(id int)(consegna_fornitore) {
	var cfn consegna_fornitore
	cfn.id=id
	cfn.qta=rand.Intn(MAX) 
	cfn.tipo_articolo = rand.Intn(3)
	return cfn
}


func cliente(id int) {
	tipo_cliente = rand.Intn(2)
	var rng richiesta_negoziante
	if tipo_cliente==NEG {
		rng = new_req_negoziante(id)
		//fmt.Printf("negoziante %d| [%d][%d] faccio richiesta\n",id,rng.qta,rng.tipo_articolo)
		req_negoziante<-rng
	} else {
		//fmt.Printf("privato %d| faccio richiesta \n", id)
		req_privato <- id
	}
	<-res_cliente[id]

	//fmt.Printf("cliente [%d] servito se ne va\n",id)
	done<-true
}

func fornitore(id int) {
	cfn:=new_cons_fornitore(id)
	req_fornitore[cfn.tipo_articolo]<-cfn
	<-res_fornitore
}


func magazzino() {
	//for, select e poi sono cazzi tuoi!
	var n_martelli,ntenaglie,n_badili int =0
	for  {
		select {
		//arrivo richiesta privato martello
		case x := <-when(n_martelli>0 && len(req_negoziante[0]==0) && len(req_negoziante[1]==0) && len(req_negoziante[2]==0),req_privato[martello]):
			n_martelli--
			<-res_cliente[x.id]

		//arrivo richiesta privato tenaglie
		case x := <-when(n_tenaglie>0 && len(req_negoziante[0]==0) && len(req_negoziante[1]==0) &&	len(req_negoziante[2]==0),req_privato[tenaglie]):
			n_tenaglie--
			<-res_cliente[x.id]

		//arrivo richiesta privato badile
		case x := <-when(n_badili>0 && len(req_negoziante[0]==0) && len(req_negoziante[1]==0) && len(req_negoziante[2]==0),req_privato[badile]):
			n_badili--
			<-res_cliente[x.id]

		//arrivo richiesta negoziante per martello
		case x := <-when(n_martelli>K+3,req_negoziante[martelli]):
			n_martelli-= x.qta
			<-res_cliente[x.id]

		//arrivo richiesta negoziante per tenaglia
		case x := <-when(n_tenaglie>=K+3 ,req_negoziante[tenaglia]):
			n_tenaglie -= x.qta
			<-res_cliente[x.id]

		//arrivo richiesta negoziante per badile
		case x := <-when(n_badili>=K+3 ,req_negoziante[badile]):
			n_badili-= x.qta
			<-res_cliente[x.id]

		case x:= <-when(,req_fornitore[martello]):
			<-res_fornitore[x.id]

		case x:= <-when(1,req_fornitore[tenaglia]):
			<-res_fornitore[x.id]

		case x:= <-when(1,req_fornitore[badile]):
			<-res_fornitore[x.id]
			

		case <-termina:
			fmt.Println("FINE !!!!!!")
			done <- true
			return
		}
	}
}

func main() {

	var n_clienti,n_fornitori int

	fmt.Printf("+++++++		start main		+++++++\n")

	fmt.Printf("inserisci n clienti (max %d)\n",MAX_CLIENTI)
	fmt.Scanf("%d\n",&n_clienti)

	fmt.Printf("inserisci n fornitori (max %d)\n",MAX_FORNITORI)
	fmt.Scanf("%d\n",&n_fornitori)

	for i := 0; i < 3; i++ {
		req_fornitore[i]=make(chan consegna_fornitore,MB)
	}

	for i := 0; i < 3; i++ {
		req_privato[i]=make(chan consegna_fornitore,MB)
	}

	for i := 0; i < 3; i++ {
		req_negoziante[i]=make(chan consegna_fornitore,MB)
	}

	for i := 0; i < MAX_CLIENTI; i++ {
		res_cliente[i]= make(chan int)
	}

	for i := 0; i < MAX_FORNITORI; i++ {
		res_fornitore[i]=make(chan int)
	}

	go magazzino()

	for i := 0; i < n_clienti; i++ {
		go cliente(i)
	}
	
	for i := 0; i < n_fornitori; i++ {
		go fornitore(i)
	}

	for i := 0; i < n_clienti+n_fornitori; i++ {
		<-done
	}
	termina<-true
	<-done
	fmt.Printf("+++++++		end main		+++++++\n")
	

}
