package main

import (
	"bufio"
	"encoding/json"
	//"io"
	"os"
	"strings"
)

type Standard struct {
	ID                 int     `json:"ID"`
	ProofTypeID        int     `json:"PTID"`
	ProofTypeOwner     string  `json:"PTOwner"`
	ProofTypeUser      string  `json:"PTUser"`
	MStandardLow       float64 `json:"MSTL"`
	MStandardHigh      float64 `json:"MSTH"`
	MStandardPrecision int     `json:"MSTP"`
	CypheredMGT        string  `json:"CMGT"`
}

type jsonReaderError string

func closeFile(file *os.File) {
	err := file.Close()
	if err != nil {
		//log.Fatal(err)
	}
}

// Error of jsonReaderError returns an interface error{}.
func (e jsonReaderError) Error() string {
	return string(e)
}

//multipleJsonCheck checks the first elements of reader []bytes, and returns boolean multipleJson true if there is a JSON array.
func multipleJsonCheck(reader *bufio.Reader) (multipleJson bool, checkError error) {
	f, err := reader.Peek(1)
	if err != nil || len(f) != 1 {
		e := jsonReaderError("peek error: " + err.Error())
		return false, e
	}
	// Peek to see if this is a JSON array.
	switch f[0] {
	case '{':
		multipleJson = false
	case '[':
		multipleJson = true
	default:
		e := jsonReaderError("expected JSON array or hash")
		return false, e
	}
	return multipleJson, nil
}

//getJson reads JSON file from os, and transforms it to a struct.
func getJson(jsonFile string, st *[]Standard) error {

	file:=strings.NewReader(jsonFile)
	//file, err := os.Open(jsonFile)
	//if err != nil {
	//	return err
	//}
	//defer closeFile(file)
	inputReader := bufio.NewReader(file)
	//dec := json.NewDecoder(inputReader)
	//if err := dec.Decode(&(*st[0])); err!=nil {
	//	e := jsonReaderError("json array decode error:"+err.Error())
	//	return e
	//}
	//var s string
	//for {
	//	inputString, readerError := inputReader.ReadString('\n')
	//	if readerError == io.EOF {
	//		break
	//	}
	//	s = s + inputString
	//}
	//fmt.Printf("all: \n%s", s)
	multiple, err := multipleJsonCheck(inputReader)
	if err != nil {
		return err
	}
	//如果是多个对象{}组成的数组[],那么就解码到standards
	if dec := json.NewDecoder(inputReader); multiple {
		if err := dec.Decode(&st); err != nil {
			e := jsonReaderError("json array decode error")
			return e
		}
	} else { //否则，解码到standards[0]
		var s0 Standard
		if err = dec.Decode(&s0); err != nil {
			e := jsonReaderError("json object decode error")
			return e
		}
		*st = append(*st, s0)
	}
	return nil
}
