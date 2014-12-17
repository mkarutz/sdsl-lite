package main

import "os"
import "log"
import "path/filepath"
import "encoding/json"
import "os/exec"
import "strconv"
import "strings"

type TestFile struct {
	Name string `json:"name"`
	Url string  `json:"url"`
	NumBytes int `json:"numbytes"`
}

func main() {
	// find build binaries
	buildBinaries,err := filepath.Glob("./bin/build*.x")
	log.Printf("Building indexes %v",buildBinaries)

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
		inputFileName := "../data/"+testFileDesc.Name
		if _, err := os.Stat(inputFileName); os.IsNotExist(err) {
			// input does not exist
		} else {
			for _,buildCmd := range buildBinaries {
				indexName := strings.TrimPrefix(buildCmd,"bin/build-")
				indexName = strings.TrimSuffix(indexName,".x")+".idx"
				indexOutFile := "./indexes/"+testFileDesc.Name+"-"+indexName
				if _, err := os.Stat(indexOutFile); os.IsNotExist(err) {
					log.Printf("Building index '%v' for file '%v'",indexName,testFileDesc.Name)
					numBytesStr := strconv.Itoa(testFileDesc.NumBytes)
					cmd := exec.Command(buildCmd,inputFileName,"../tmp/",indexOutFile,numBytesStr)
					cmd.Stdout = os.Stdout
					err := cmd.Run()
					if err != nil {
						log.Printf("Error building index: %v",err)
					}
					log.Printf("Done building index '%v'",indexOutFile)
					} else {
						log.Printf("Index '%v' exists. Delete if you want to rebuild.",indexOutFile)
					}
			}

		}
	}
}