package main

import (
	"fmt"
	//"math/rand"
	//"time"
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

func init_chan() {
	
}

func print_chan_len() {
	
}

func main() {

	init_chan()

	fmt.Printf("FINE\n")
}
