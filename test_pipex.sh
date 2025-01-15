#!/bin/bash

# Cores para output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
NC='\033[0m'

# Cria diretório de teste se não existir
mkdir -p test_files

# Cria arquivo de entrada para testes
echo "Hello World!" > test_files/input.txt
echo "Another line" >> test_files/input.txt
echo "Testing pipex" >> test_files/input.txt
echo "Hello again" >> test_files/input.txt

# Função para comparar resultados
compare_outputs() {
    echo -e "${BLUE}Test: $1${NC}"
    # Executa comando com bash
    eval < test_files/input.txt $2 > test_files/expected.txt
    # Executa com pipex
    ./pipex test_files/input.txt "${3}" "${4}" test_files/output.txt
    
    # Compara resultados
    if diff test_files/expected.txt test_files/output.txt > /dev/null; then
        echo -e "${GREEN}✓ Test passed${NC}"
    else
        echo -e "${RED}✗ Test failed${NC}"
        echo "Expected:"
        cat test_files/expected.txt
        echo "Got:"
        cat test_files/output.txt
    fi
    echo "----------------------------------------"
}

# Testes básicos
echo "Running basic tests..."

# Teste 1: grep e wc
compare_outputs "grep + wc" \
    "grep Hello | wc -l" \
    "grep Hello" "wc -l"

# Teste 2: cat e grep
compare_outputs "cat + grep" \
    "cat | grep Hello" \
    "cat" "grep Hello"

# Teste 3: grep e sort
compare_outputs "grep + sort" \
    "grep 'e' | sort" \
    "grep 'e'" "sort"

# Testes com here_doc
echo -e "${BLUE}Testing here_doc...${NC}"
./pipex here_doc EOF "cat" "grep Hello" test_files/output.txt << EOF
Hello World
Testing here_doc
Hello again
EOF

# Testes de erro
echo -e "${BLUE}Testing error cases...${NC}"

# Teste com comando inválido
./pipex test_files/input.txt "invalid_cmd" "wc -l" test_files/output.txt
if [ $? -eq 127 ]; then
    echo -e "${GREEN}✓ Invalid command test passed${NC}"
else
    echo -e "${RED}✗ Invalid command test failed${NC}"
fi

# Teste com arquivo inexistente
./pipex nonexistent.txt "cat" "wc -l" test_files/output.txt
if [ $? -ne 0 ]; then
    echo -e "${GREEN}✓ Nonexistent file test passed${NC}"
else
    echo -e "${RED}✗ Nonexistent file test failed${NC}"
fi

# Limpa arquivos de teste
rm -rf test_files

echo -e "${BLUE}Tests completed${NC}"