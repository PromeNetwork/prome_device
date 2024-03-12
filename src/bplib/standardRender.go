package main

import (
	"errors"
	"fmt"
	"log"
	"math"
)

type RenderedStandard struct {
	low       int
	high      int
	precision int
}

//RenderedStandard.Low returns rendered low standard.
func (r *RenderedStandard) Low() int {
	return r.low
}

//RenderedStandard.High returns rendered high standard.
func (r *RenderedStandard) High() int {
	return r.high
}

//RenderedStandard.Precision returns standard precision.
func (r *RenderedStandard) Precision() int {
	return r.precision
}

//RenderedStandard.setLow sets rendered low standard.
func (r *RenderedStandard) setLow(st Standard) {
	var x float64
	if st.MStandardPrecision >= 0 {
		x = math.Pow10(st.MStandardPrecision)
	}
	r.low = int(math.Trunc(x * st.MStandardLow))
}

//RenderedStandard.setHigh sets rendered high standard.
func (r *RenderedStandard) setHigh(st Standard) {
	var x float64
	if st.MStandardPrecision >= 0 {
		x = math.Pow10(st.MStandardPrecision)
	}
	r.high = int(math.Trunc(x * st.MStandardHigh))
}

//RenderedStandard.setPrecision sets standard precision with same value from data reader.
func (r *RenderedStandard) setPrecision(st Standard) {
	r.precision = st.MStandardPrecision
}

//omitID omits Standard.ID and some elements of the struct, to detect if there's repeated Standard.
func omitID(withID Standard) (withoutID string) {
	str := string(rune(withID.ProofTypeID)) + withID.ProofTypeOwner + withID.ProofTypeUser
	return str
}

//reportRepeatedElement reports repeated element with same Standard.ProofTypeID.
func reportRepeatedElement(i int, arr []Standard, removable *bool) {
	if *removable != false {
		return
	}
	for j := i + 1; j < len(arr); j++ {
		strI := omitID(arr[i])
		strJ := omitID(arr[j])
		if arr[i] == arr[j] || strI == strJ {
			*removable = true
			msg := fmt.Sprintf("ID: %v 's standard has repeated elements with ID: %v.", arr[i].ID, arr[j].ID)
			log.Println(msg)
			return
		}
	}
	return
}

//reportErrorStandard reports Standard which is mistaken define.
func reportErrorStandard(i int, arr []Standard, removable *bool) {
	if *removable != false {
		return
	}
	if arr[i].MStandardLow >= arr[i].MStandardHigh {
		*removable = true
		msg := fmt.Sprintf("ID: %v 's StandardLow is not Lower than StandardHigh.", arr[i].ID)
		log.Println(msg)
	}
	return
}

//reportUnusableStandard reports Standard which not relevant to specific user.
func reportUnusableStandard(i int, arr []Standard, user string, removable *bool) {
	if *removable != false {
		return
	}
	if arr[i].ProofTypeUser != user {
		*removable = true
		msg := fmt.Sprintf("Removing ID:  %v 's standard, because it is irrelevant to user.", arr[i].ID)
		log.Println(msg)
	}
	return
}

//StandardCleaning cleans raw standard with deduplicate func,etc.. and provides Standard for init.
func StandardCleaning(arr []Standard) (newArr []Standard) {
	newArr = make([]Standard, 0)
	for i := 0; i < len(arr); i++ {
		var removable bool
		removable = false
		reportErrorStandard(i, arr, &removable)
		reportUnusableStandard(i, arr, ProofUser, &removable)
		reportRepeatedElement(i, arr, &removable)
		if !removable {
			newArr = append(newArr, arr[i])
		}
	}
	return newArr
}

//StandardPreparing reads data from JSON reader and returns a map of RenderedStandard.
func StandardPreparing(arr []Standard, ProofStandardMap map[int]RenderedStandard) error {
	newArr := StandardCleaning(arr)
	if len(newArr) == 0 {
		err := errors.New("there is no element in array after rendering. no standard could be used for current user")
		return err
	}
	for _, value := range newArr {
		var rs RenderedStandard
		rs.setLow(value)
		rs.setHigh(value)
		rs.setPrecision(value)
		i := value.ProofTypeID
		ProofStandardMap[i] = rs
	}
	return nil
}
