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


