
package main

import "os"
import "log"
import "path/filepath"
import "encoding/json"
import "strings"
import "fmt"

const cmake_output_file = "./config/indexes/parsed_index_configs.cmake"

type IndexConfig struct {
	Name string `json:"name"`
	CompileDefs map[string]interface{} `json:"compiler_defines"`
	Libraries []string `json:"libraries"`
}

func AddCMakeTarget(cmakeFile *os.File,binPrefix string,cppFile string,cdefs []string ,cdef_str []string,libs []string) {
	log.Printf("Generate build target '%v'",binPrefix)
	fmt.Fprintf(cmakeFile,"add_executable(%v.x %v)\n",binPrefix,cppFile)
	fmt.Fprintf(cmakeFile,"target_link_libraries(%v.x %v)\n",binPrefix,strings.Join(libs," "))
	fmt.Fprintf(cmakeFile,"set_property(TARGET %v.x PROPERTY COMPILE_DEFINITIONS %v %v)\n",binPrefix,
				strings.Join(cdefs," "),strings.Join(cdef_str," "))
	fmt.Fprintf(cmakeFile,"list(APPEND BUILD_BINARIES %v.x)\n\n",binPrefix)
}

func AddCMakeTargets(cmakeFile *os.File,idxCfg IndexConfig) {
	cppFiles,err := filepath.Glob("./src/*.cpp")
	if err != nil {
		log.Fatalf("Glob file error %v",err)
	}
	for _,cppFile := range cppFiles {
		binPrefix := strings.TrimPrefix(cppFile, "src/")
		binPrefix = "bin/"+strings.TrimSuffix(binPrefix, ".cpp")+"-"+idxCfg.Name
		var compile_defs []string
		var compile_defs_str []string
		var has_parameters bool = false
		for k,v := range idxCfg.CompileDefs {
			switch val := v.(type) {
				case string:
					compile_defs = append(compile_defs,fmt.Sprintf("%v=%v",k,val))
					compile_defs_str = append(compile_defs_str,fmt.Sprintf("%v_STR=\"%v\"",k,val))
				case bool:
					compile_defs = append(compile_defs,fmt.Sprintf("%v=%v",k,val))
					compile_defs_str = append(compile_defs_str,fmt.Sprintf("%v_STR=\"%v\"",k,val))
				case float64:
					compile_defs = append(compile_defs,fmt.Sprintf("%v=%v",k,val))
					compile_defs_str = append(compile_defs_str,fmt.Sprintf("%v_STR=\"%v\"",k,val))
				case []interface{}:
					has_parameters = true
				default:
					log.Fatalf("Don't know how to handle %v:%v\n",k,v)
			}
		}
		// always add name as compiler def
		compile_defs_str = append(compile_defs_str,fmt.Sprintf("NAME=\"%v\"",idxCfg.Name))

		// parse parameters if any
		if has_parameters == true {
			for k,v := range idxCfg.CompileDefs {
				switch val := v.(type) {
					case []interface{}:
						for _,vv := range val {
							binParPrefix := binPrefix+fmt.Sprintf("-%v-%v",k,vv)
							compile_defs_par := compile_defs
							compile_defs_par_str := compile_defs_str
							compile_defs_par = append(compile_defs_par,fmt.Sprintf("%v=%v",k,vv))
							compile_defs_par_str = append(compile_defs_par_str,fmt.Sprintf("%v_STR=\"%v\"",k,vv))
							compile_defs_par_str = append(compile_defs_par_str,fmt.Sprintf("PARAM_STR=\"%v-%v=%v\"",idxCfg.Name,k,vv))
							AddCMakeTarget(cmakeFile,binParPrefix,cppFile,compile_defs_par,compile_defs_par_str,idxCfg.Libraries)
						}
				}
			}
		} else {
			// only one config
			AddCMakeTarget(cmakeFile,binPrefix,cppFile,compile_defs,compile_defs_str,idxCfg.Libraries)
		}
	}
}

func main() {
	indexConfigFiles,err := filepath.Glob("./config/indexes/*.json")
	if err != nil {
		log.Fatalf("Glob file error %v",err)
	}

	cmakeFile, err := os.Create(cmake_output_file)
	if err != nil {
		log.Fatal(err)
	}


	for _,indexDesc := range indexConfigFiles {
		log.Printf("Generating cmake targets from %v",indexDesc)
		jFile, err := os.Open(indexDesc)
		if err != nil {
			log.Fatal(err)
		}
		dec := json.NewDecoder(jFile)
		var indexConfig IndexConfig
		err = dec.Decode(&indexConfig)
		if err != nil {
			log.Fatalf("index config decoding error %v",err)
		} else {
			AddCMakeTargets(cmakeFile,indexConfig)
		}
	}
}