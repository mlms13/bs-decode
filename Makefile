project_name = bs-decode

DUNE = opam exec -- dune

.DEFAULT_GOAL := help

.PHONY: help
help: ## Print this help message
	@echo "List of available make commands";
	@echo "";
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | awk 'BEGIN {FS = ":.*?## "}; {printf "  \033[36m%-15s\033[0m %s\n", $$1, $$2}';
	@echo "";

.PHONY: create-switch
create-switch: ## Create opam switch
	opam switch create . -y --deps-only

.PHONY: init
init: create-switch install ## Configure everything to develop this repository in local

.PHONY: install
install: ## Install development dependencies
	yarn install
	opam install -y . --deps-only
	opam pin -y add $(project_name).dev .
	rm -rf node_modules/melange && ln -sfn $$(opam var melange:lib)/runtime node_modules/melange

.PHONY: build
build: ## Build the project
	$(DUNE) build @main

.PHONY: watch
watch: ## Watch for the filesystem and rebuild on every change
	$(DUNE) build --watch @main

.PHONY: clean
clean: ## Clean build artifacts and other generated files
	rm test/output/test/utils/BigJson.json
	$(DUNE) clean

.PHONY: copy-test-json
copy-test-json: ## Copy test json files to the right place
	cp test/utils/BigJson.json test/output/test/utils/BigJson.json

.PHONY: test
test: copy-test-json
	yarn test

.PHONY: test-coverage
test-coverage: copy-test-json
	yarn test --coverage

.PHONY: format
format: ## Format the codebase with ocamlformat
	$(DUNE) build @fmt --auto-promote

.PHONY: format-check
format-check: ## Checks if format is correct
	$(DUNE) build @fmt
