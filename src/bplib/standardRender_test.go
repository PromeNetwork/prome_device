package main

import (
	"fmt"
	"testing"
)

func TestStandardPreparing(t *testing.T) {
	structTestList := []Standard{
		{1, 1, "Governor", " ", 90, 30, 2, "alpha"},
		{2, 2, "Governor", " ", 10, 50, 2, "alpha"},
		{3, 2, "Governor", "MSTH", 10, 50, 2, "alpha"},
		{4, 9, "Governor", " ", 1000, 6999, 1, "alpha"},
		{5, 2, "Governor", " ", 100, 505, 2, "alpha"},
	}
	got := make(map[int]RenderedStandard)
	want := map[int]RenderedStandard{2: {10000, 50500, 2}, 9: {10000, 69990, 1}}
	err := StandardPreparing(structTestList, got)
	if err != nil {
		t.Error(err.Error())
	}
	for key := range got {
		//fmt.Println(key)
		//fmt.Println(got[key])
		//fmt.Println(want[key])
		if got[key] != want[key] {
			t.Error(errorRenderString(structTestList, want[key], got[key]))
		}
	}
}

func errorRenderString(structTestList []Standard, wantElement RenderedStandard, gotElement RenderedStandard) string {
	return fmt.Sprintf("The test struct list is: %v\n Want: %v\n Got: %v\n", structTestList, wantElement, gotElement)
}
