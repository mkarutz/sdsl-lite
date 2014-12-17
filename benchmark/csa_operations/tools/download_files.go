package main

import "net/http"
import "io"
import "os"
import "log"
import "path/filepath"
import "encoding/json"
import "compress/gzip"
import "compress/bzip2"
import "strings"

type TestFile struct {
	Name string `json:"name"`
	Url string  `json:"url"`
	IntegerFile bool `json:"integerfile"`
}

func main() {
	log.Printf("Download test files if they don't exist.")
	log.Printf("Parsing test file descriptions in './config/files'")
	testConfigFile,err := filepath.Glob("./config/files/*.json")
	if err != nil {
		log.Fatalf("Glob file error %v",err)
	}

	for _,fileDesc := range testConfigFile {
		log.Printf("Parsing test file description '%v'",fileDesc)
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
			log.Printf("Downloading file '%v' from '%v'",testFileDesc.Name,testFileDesc.Url)
			out, err := os.Create(outFileName)
			defer out.Close()
			resp, err := http.Get(testFileDesc.Url)
			if err != nil {
				log.Fatalf("file download error %v",err)
			}
			defer resp.Body.Close()
			if strings.HasSuffix(testFileDesc.Url,".gz") {
				log.Printf("Extracting using gzip to '%v'",outFileName)
				r, err := gzip.NewReader(resp.Body)
				if err != nil {
					log.Fatalf("file gzip error %v",err)
				}
				_, err = io.Copy(out, r)
				if err != nil {
					log.Fatalf("file store error %v",err)
				}
			} else if strings.HasSuffix(testFileDesc.Url,".bz2") {
				log.Printf("Extracting using bzip2 to '%v'",outFileName)
				r := bzip2.NewReader(resp.Body)
				if err != nil {
					log.Fatalf("file bzip2 error %v",err)
				}
				_, err = io.Copy(out, r)
				if err != nil {
					log.Fatalf("file store error %v",err)
				}
			} else {
				_, err = io.Copy(out, resp.Body)
				if err != nil {
					log.Fatalf("file store error %v",err)
				}
			}

		} else {
			log.Printf("File '%v' already exists.",outFileName)
		}
	}
}