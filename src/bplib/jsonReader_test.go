package main

import (
	"fmt"
	"testing"
)

//TestJSONArray tests if JsonReader could decode a JSON array.
func TestJSONArray(t *testing.T) {
	jsonFile := "./standard.json"
	var got []Standard
	want := []Standard{
		{1, 1, "Governor", "WXblockchain", 10, 40, 2, "alpha"},
		{2, 2, "Governor", "WXblockchain", 10, 50, 2, "alpha"},
		{3, 2, "Governor", "MSTH", 10, 50, 2, "alpha"},
		{4, 9, "Governor", "WXblockchain", 1000, 6999, 1, "alpha"},
	}
	err := getJson(jsonFile, &got)
	if err != nil {
		t.Error(err)
	}
	for i := range want {
		if got[i] != want[i] {
			t.Error(errorString(jsonFile, want[i], got[i]))
		}
	}
	//fmt.Print(got)
}

//TestJSON tests if JsonReader could decode one JSON object.
func TestJSON(t *testing.T) {
	jsonFile := "./standard2.json"
	var got []Standard
	want := []Standard{
		{1, 1, "Governor", "WXblockchain", 10, 40, 2, "alpha"},
	}
	err := getJson(jsonFile, &got)
	if err != nil {
		t.Error(err)
	}
	if got[0] != want[0] {
		t.Error(errorString(jsonFile, want[0], got[0]))
	}
	//fmt.Print(got)
}

func errorString(jsonFile string, wantElement Standard, gotElement Standard) string {
	return fmt.Sprintf("The JSON file is: %s\n Want: %v Got: %v", jsonFile, wantElement, gotElement)
}
