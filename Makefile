project_name = bs-decode

DUNE = opam exec -- dune
MEL = opam exec -- mel

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
	npm install
	opam install -y . --deps-only
	opam pin -y add $(project_name).dev .
	rm -rf node_modules/melange && ln -sfn $$(opam var melange:lib)/runtime node_modules/melange

.PHONY: build
build: ## Build the project
	$(MEL) build

.PHONY: clean
clean: ## Clean build artifacts and other generated files
	$(MEL) clean

.PHONY: test-coverage
test-coverage: ## Run the tests (producing a coverage report)
	jest --coverage


.PHONY: format
format: ## Format the codebase with ocamlformat
	$(DUNE) build @fmt --auto-promote

.PHONY: format-check
format-check: ## Checks if format is correct
	$(DUNE) build @fmt

.PHONY: watch
watch: ## Watch for the filesystem and rebuild on every change
	$(MEL) build --watch
