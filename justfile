alias b := build
alias c := clean
alias ga := gui_assess
alias gd := gui_data
alias h := hotspot

gui_assess *args:
    @just profile assess_ext ./main.exe {{args}}
    @just gui_view
gui_data *args:
    @just profile data_access ./main.exe {{args}}
    @just gui_view

hotspot *args: (perf_profile "./main.exe" args)
    hotspot --sourcePaths . --appPath . output/perf.data 

gui_view:
    #!/bin/sh
    AMDuProf --session /tmp/prof/$(ls /tmp/prof) --src-path $(realpath .) --bin-path $(realpath .)

tui_view:
    #!/bin/sh
    AMDuProfCLI report -i /tmp/prof/$(ls /tmp/prof) --src-path $(realpath .) --bin-path $(realpath .)
    vd /tmp/prof/$(ls /tmp/prof)/report.csv

profile mode *args: build
    #!/bin/sh
    rm -rf /tmp/prof/*
    echo {{args}}
    AMDuProfCLI collect --config {{mode}} -o /tmp/prof {{args}}

perf_profile *args: build
    perf record -o output/perf.data -e cache-misses,cycles,page-faults --call-graph dwarf {{args}}
    
build:
    make -f OriginalMakefile all

clean:
    make -f OriginalMakefile clean
