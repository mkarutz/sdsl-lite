package main

import "os"
import "log"
import "path/filepath"
import "encoding/json"
import "fmt"

type TestFile struct {
	Name string `json:"name"`
	Url string  `json:"url"`
	IntegerFile bool `json:"integerfile"`
}

func main() {
	testConfigFile,err := filepath.Glob("./config/files/*.json")
	if err != nil {
		log.Fatalf("Glob file error %v",err)
	}

	for _,fileDesc := range testConfigFile {
		jFile, err := os.Open(fileDesc)
		if err != nil {
			log.Fatal(err)
		}
		dec := json.NewDecoder(jFile)
		var testFileDesc TestFile
		err = dec.Decode(&testFileDesc)
		if err != nil {
			log.Fatalf("test config decoding error %v",err)
		}
		outFileName := "../data/"+testFileDesc.Name
		if _, err := os.Stat(outFileName); os.IsNotExist(err) {

		} else {
			fmt.Printf("%v;",outFileName)
		}
	}
}