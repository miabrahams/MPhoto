
OUTPUT_DIR=../../doc

.PHONY: doc
doc: $(OUTPUT_DIR)
	zim --export --template html_template.html -o $(OUTPUT_DIR) notebook.zim

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)
