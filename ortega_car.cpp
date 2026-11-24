/*
 * ============================================================================
 * PROJETO DE EXTENSAO 1 (CMPEXT1) - TADS-EAD IFSP (Polo Sao Paulo)
 * Projeto: Ortega Car - Sistema de Controle de Servicos e Clientes
 * Grupo:   Grupo Jd. Paulistano - Projeto Ortega Car
 * Integrantes:
 *   - Mariana Lubarino Vilas Boas (CP3072371)
 *   - Danilo Fernandes Vilas Boas (CP3073866)
 *   - Gilmar Silva Neves (CP3072193)
 *   - Vinicius Henrique Ortega da Silva (CP3072363)
 *   - Micael das Chagas Pereira Nascimento (CP3074099)
 * Demandante: Ortega Car - Oficina Mecanica Automotiva (Cesar Augusto)
 * Finalidade: Esboco minimamente funcional em modo console com programacao
 *             estruturada e persistencia local em arquivos CSV.
 * ============================================================================
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <limits>

using namespace std;

// ============================================================================
// ESTRUTURAS DE DADOS (Registros do Sistema)
// ============================================================================

struct Cliente {
    string cpf;
    string nome;
    string telefone;
    string veiculoPrincipal;
};

struct Veiculo {
    string placa;
    string cpfCliente;
    string marca;
    string modelo;
    int ano;
};

struct PecaItem {
    int idOS;
    string nome;
    double valorUnitario;
    int quantidade;
};

struct OrdemServico {
    int id;
    string cpfCliente;
    string placaVeiculo;
    string dataEntrada;
    string dataConclusao;
    string relatoFalha;
    string status; // "Em Analise", "Aprovada", "Em Manutencao", "Concluida", "Cancelada"
    double maoDeObra;
    double valorTotal;
};

// Nomes dos arquivos CSV para persistencia local (RNF02)
const string ARQ_CLIENTES = "clientes.csv";
const string ARQ_VEICULOS = "veiculos.csv";
const string ARQ_OS       = "ordens_servico.csv";
const string ARQ_PECAS    = "itens_os.csv";

// ============================================================================
// FUNCOES UTILITARIAS DE ENTRADA E FORMATACAO
// ============================================================================

string limparTextoCSV(string texto) {
    for (size_t i = 0; i < texto.size(); ++i) {
        if (texto[i] == ';') texto[i] = ',';
        if (texto[i] == '\n' || texto[i] == '\r') texto[i] = ' ';
    }
    return texto;
}

string paraMaiusculas(string texto) {
    for (size_t i = 0; i < texto.size(); ++i) {
        texto[i] = static_cast<char>(toupper(static_cast<unsigned char>(texto[i])));
    }
    return texto;
}

string lerLinha(const string& prompt) {
    string entrada;
    cout << prompt;
    getline(cin, entrada);
    return limparTextoCSV(entrada);
}

int lerInteiro(const string& prompt) {
    int valor;
    while (true) {
        cout << prompt;
        if (cin >> valor) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }
        cout << "[ERRO] Entrada invalida. Digite um numero inteiro.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

double lerDouble(const string& prompt) {
    double valor;
    while (true) {
        cout << prompt;
        if (cin >> valor && valor >= 0.0) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return valor;
        }
        cout << "[ERRO] Entrada invalida. Digite um valor numerico maior ou igual a zero.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

void pausar() {
    cout << "\nPressione ENTER para continuar...";
    string dummy;
    getline(cin, dummy);
}

void exibirCabecalho(const string& titulo) {
    cout << "\n============================================================\n";
    cout << "  ORTEGA CAR - " << titulo << "\n";
    cout << "============================================================\n";
}

// ============================================================================
// FUNCOES DE PERSISTENCIA EM ARQUIVOS CSV (RNF02)
// ============================================================================

void salvarClientes(const vector<Cliente>& clientes) {
    ofstream arq(ARQ_CLIENTES);
    if (!arq.is_open()) {
        cout << "[ERRO] Nao foi possivel salvar " << ARQ_CLIENTES << ".\n";
        return;
    }
    arq << "cpf;nome;telefone;veiculo_principal\n";
    for (size_t i = 0; i < clientes.size(); ++i) {
        arq << clientes[i].cpf << ";"
            << clientes[i].nome << ";"
            << clientes[i].telefone << ";"
            << clientes[i].veiculoPrincipal << "\n";
    }
}

void carregarClientes(vector<Cliente>& clientes) {
    clientes.clear();
    ifstream arq(ARQ_CLIENTES);
    if (!arq.is_open()) return;

    string linha;
    getline(arq, linha); // Ignora cabecalho
    while (getline(arq, linha)) {
        if (linha.empty()) continue;
        stringstream ss(linha);
        Cliente c;
        getline(ss, c.cpf, ';');
        getline(ss, c.nome, ';');
        getline(ss, c.telefone, ';');
        getline(ss, c.veiculoPrincipal, ';');
        if (!c.cpf.empty()) clientes.push_back(c);
    }
}

void salvarVeiculos(const vector<Veiculo>& veiculos) {
    ofstream arq(ARQ_VEICULOS);
    if (!arq.is_open()) {
        cout << "[ERRO] Nao foi possivel salvar " << ARQ_VEICULOS << ".\n";
        return;
    }
    arq << "placa;cpf_cliente;marca;modelo;ano\n";
    for (size_t i = 0; i < veiculos.size(); ++i) {
        arq << veiculos[i].placa << ";"
            << veiculos[i].cpfCliente << ";"
            << veiculos[i].marca << ";"
            << veiculos[i].modelo << ";"
            << veiculos[i].ano << "\n";
    }
}

void carregarVeiculos(vector<Veiculo>& veiculos) {
    veiculos.clear();
    ifstream arq(ARQ_VEICULOS);
    if (!arq.is_open()) return;

    string linha;
    getline(arq, linha); // Ignora cabecalho
    while (getline(arq, linha)) {
        if (linha.empty()) continue;
        stringstream ss(linha);
        Veiculo v;
        string anoStr;
        getline(ss, v.placa, ';');
        getline(ss, v.cpfCliente, ';');
        getline(ss, v.marca, ';');
        getline(ss, v.modelo, ';');
        getline(ss, anoStr, ';');
        v.ano = anoStr.empty() ? 0 : stoi(anoStr);
        if (!v.placa.empty()) veiculos.push_back(v);
    }
}

void salvarPecas(const vector<PecaItem>& pecas) {
    ofstream arq(ARQ_PECAS);
    if (!arq.is_open()) {
        cout << "[ERRO] Nao foi possivel salvar " << ARQ_PECAS << ".\n";
        return;
    }
    arq << "id_os;nome_peca;valor_unitario;quantidade\n";
    for (size_t i = 0; i < pecas.size(); ++i) {
        arq << pecas[i].idOS << ";"
            << pecas[i].nome << ";"
            << fixed << setprecision(2) << pecas[i].valorUnitario << ";"
            << pecas[i].quantidade << "\n";
    }
}

void carregarPecas(vector<PecaItem>& pecas) {
    pecas.clear();
    ifstream arq(ARQ_PECAS);
    if (!arq.is_open()) return;

    string linha;
    getline(arq, linha); // Ignora cabecalho
    while (getline(arq, linha)) {
        if (linha.empty()) continue;
        stringstream ss(linha);
        PecaItem p;
        string idStr, valStr, qtdStr;
        getline(ss, idStr, ';');
        getline(ss, p.nome, ';');
        getline(ss, valStr, ';');
        getline(ss, qtdStr, ';');
        if (!idStr.empty()) {
            p.idOS = stoi(idStr);
            p.valorUnitario = valStr.empty() ? 0.0 : stod(valStr);
            p.quantidade = qtdStr.empty() ? 0 : stoi(qtdStr);
            pecas.push_back(p);
        }
    }
}

void salvarOrdens(const vector<OrdemServico>& ordens) {
    ofstream arq(ARQ_OS);
    if (!arq.is_open()) {
        cout << "[ERRO] Nao foi possivel salvar " << ARQ_OS << ".\n";
        return;
    }
    arq << "id;cpf_cliente;placa_veiculo;data_entrada;data_conclusao;relato_falha;status;mao_de_obra;valor_total\n";
    for (size_t i = 0; i < ordens.size(); ++i) {
        arq << ordens[i].id << ";"
            << ordens[i].cpfCliente << ";"
            << ordens[i].placaVeiculo << ";"
            << ordens[i].dataEntrada << ";"
            << ordens[i].dataConclusao << ";"
            << ordens[i].relatoFalha << ";"
            << ordens[i].status << ";"
            << fixed << setprecision(2) << ordens[i].maoDeObra << ";"
            << fixed << setprecision(2) << ordens[i].valorTotal << "\n";
    }
}

void carregarOrdens(vector<OrdemServico>& ordens) {
    ordens.clear();
    ifstream arq(ARQ_OS);
    if (!arq.is_open()) return;

    string linha;
    getline(arq, linha); // Ignora cabecalho
    while (getline(arq, linha)) {
        if (linha.empty()) continue;
        stringstream ss(linha);
        OrdemServico os;
        string idStr, moStr, totStr;
        getline(ss, idStr, ';');
        getline(ss, os.cpfCliente, ';');
        getline(ss, os.placaVeiculo, ';');
        getline(ss, os.dataEntrada, ';');
        getline(ss, os.dataConclusao, ';');
        getline(ss, os.relatoFalha, ';');
        getline(ss, os.status, ';');
        getline(ss, moStr, ';');
        getline(ss, totStr, ';');
        if (!idStr.empty()) {
            os.id = stoi(idStr);
            os.maoDeObra = moStr.empty() ? 0.0 : stod(moStr);
            os.valorTotal = totStr.empty() ? 0.0 : stod(totStr);
            ordens.push_back(os);
        }
    }
}

void carregarTodosDados(vector<Cliente>& clientes, vector<Veiculo>& veiculos,
                        vector<OrdemServico>& ordens, vector<PecaItem>& pecas) {
    carregarClientes(clientes);
    carregarVeiculos(veiculos);
    carregarOrdens(ordens);
    carregarPecas(pecas);
}

// ============================================================================
// FUNCOES DE BUSCA E REGRAS DE NEGOCIO
// ============================================================================

int buscarClientePorCPF(const vector<Cliente>& clientes, const string& cpf) {
    for (size_t i = 0; i < clientes.size(); ++i) {
        if (clientes[i].cpf == cpf) return static_cast<int>(i);
    }
    return -1;
}

int buscarVeiculoPorPlaca(const vector<Veiculo>& veiculos, const string& placa) {
    string placaBusca = paraMaiusculas(placa);
    for (size_t i = 0; i < veiculos.size(); ++i) {
        if (veiculos[i].placa == placaBusca) return static_cast<int>(i);
    }
    return -1;
}

int buscarOSPorID(const vector<OrdemServico>& ordens, int id) {
    for (size_t i = 0; i < ordens.size(); ++i) {
        if (ordens[i].id == id) return static_cast<int>(i);
    }
    return -1;
}

// RF06: Calcula automaticamente o valor total da OS (pecas + mao de obra)
void recalcularTotalOS(OrdemServico& os, const vector<PecaItem>& pecas) {
    double somaPecas = 0.0;
    for (size_t i = 0; i < pecas.size(); ++i) {
        if (pecas[i].idOS == os.id) {
            somaPecas += pecas[i].valorUnitario * pecas[i].quantidade;
        }
    }
    os.valorTotal = somaPecas + os.maoDeObra;
}

int proximoID_OS(const vector<OrdemServico>& ordens) {
    int maior = 0;
    for (size_t i = 0; i < ordens.size(); ++i) {
        if (ordens[i].id > maior) maior = ordens[i].id;
    }
    return maior + 1;
}

// ============================================================================
// MODULO DE CLIENTES (RF01, RF02, RN01)
// ============================================================================

void cadastrarCliente(vector<Cliente>& clientes) {
    exibirCabecalho("CADASTRAR CLIENTE (RF01)");
    Cliente c;
    c.cpf = lerLinha("CPF do cliente (somente numeros ou formatado): ");
    if (c.cpf.empty()) {
        cout << "[ERRO] O CPF nao pode ficar em branco.\n";
        return;
    }
    // RN01: Unicidade de CPF
    if (buscarClientePorCPF(clientes, c.cpf) != -1) {
        cout << "[ERRO - RN01] Ja existe um cliente cadastrado com o CPF " << c.cpf << ".\n";
        return;
    }
    c.nome = lerLinha("Nome completo: ");
    c.telefone = lerLinha("Telefone/WhatsApp: ");
    c.veiculoPrincipal = lerLinha("Veiculo principal (ex.: Gol 1.0 Prata): ");

    clientes.push_back(c);
    salvarClientes(clientes);
    cout << "\n[OK] Cliente '" << c.nome << "' cadastrado e salvo com sucesso!\n";
}

void consultarOuListarClientes(const vector<Cliente>& clientes) {
    exibirCabecalho("CONSULTAR E LISTAR CLIENTES (RF02)");
    if (clientes.empty()) {
        cout << "Nenhum cliente cadastrado ate o momento.\n";
        return;
    }
    cout << "1 - Buscar cliente por CPF\n";
    cout << "2 - Listar todos os clientes\n";
    int op = lerInteiro("Escolha uma opcao: ");

    if (op == 1) {
        string cpf = lerLinha("Digite o CPF para busca: ");
        int idx = buscarClientePorCPF(clientes, cpf);
        if (idx == -1) {
            cout << "[AVISO] Cliente com CPF '" << cpf << "' nao encontrado.\n";
        } else {
            const Cliente& c = clientes[idx];
            cout << "\n--- Dados do Cliente ---\n";
            cout << "CPF:               " << c.cpf << "\n";
            cout << "Nome:              " << c.nome << "\n";
            cout << "Telefone:          " << c.telefone << "\n";
            cout << "Veiculo Principal: " << c.veiculoPrincipal << "\n";
        }
    } else if (op == 2) {
        cout << "\nTotal de clientes: " << clientes.size() << "\n";
        cout << "------------------------------------------------------------\n";
        for (size_t i = 0; i < clientes.size(); ++i) {
            cout << (i + 1) << ". " << clientes[i].nome
                 << " | CPF: " << clientes[i].cpf
                 << " | Tel: " << clientes[i].telefone
                 << " | Veic: " << clientes[i].veiculoPrincipal << "\n";
        }
    } else {
        cout << "[ERRO] Opcao invalida.\n";
    }
}

// ============================================================================
// MODULO DE VEICULOS (RF03, RN01)
// ============================================================================

void cadastrarVeiculo(const vector<Cliente>& clientes, vector<Veiculo>& veiculos) {
    exibirCabecalho("CADASTRAR VEICULO (RF03)");
    if (clientes.empty()) {
        cout << "[AVISO] Cadastre um cliente antes de vincular um veiculo.\n";
        return;
    }
    string cpf = lerLinha("CPF do proprietario (cliente ja cadastrado): ");
    int idxCli = buscarClientePorCPF(clientes, cpf);
    if (idxCli == -1) {
        cout << "[ERRO] Cliente nao encontrado. Cadastre o cliente primeiro.\n";
        return;
    }

    Veiculo v;
    v.cpfCliente = cpf;
    v.placa = paraMaiusculas(lerLinha("Placa do veiculo (ex.: ABC1D23): "));
    if (v.placa.empty()) {
        cout << "[ERRO] A placa nao pode ficar em branco.\n";
        return;
    }
    // RN01: Unicidade de placa
    if (buscarVeiculoPorPlaca(veiculos, v.placa) != -1) {
        cout << "[ERRO - RN01] Ja existe um veiculo cadastrado com a placa " << v.placa << ".\n";
        return;
    }
    v.marca = lerLinha("Marca (ex.: Volkswagen): ");
    v.modelo = lerLinha("Modelo (ex.: Gol 1.0): ");
    v.ano = lerInteiro("Ano de fabricacao: ");

    veiculos.push_back(v);
    salvarVeiculos(veiculos);
    cout << "\n[OK] Veiculo placa " << v.placa << " vinculado a "
         << clientes[idxCli].nome << " e salvo com sucesso!\n";
}

void listarVeiculos(const vector<Veiculo>& veiculos, const vector<Cliente>& clientes) {
    exibirCabecalho("LISTA DE VEICULOS CADASTRADOS");
    if (veiculos.empty()) {
        cout << "Nenhum veiculo cadastrado.\n";
        return;
    }
    for (size_t i = 0; i < veiculos.size(); ++i) {
        int idxCli = buscarClientePorCPF(clientes, veiculos[i].cpfCliente);
        string nomeDono = (idxCli != -1) ? clientes[idxCli].nome : "Desconhecido";
        cout << (i + 1) << ". Placa: " << veiculos[i].placa
             << " | " << veiculos[i].marca << " " << veiculos[i].modelo
             << " (" << veiculos[i].ano << ")"
             << " | Cliente: " << nomeDono << " (" << veiculos[i].cpfCliente << ")\n";
    }
}

// ============================================================================
// MODULO DE ORDENS DE SERVICO (RF04 - RF08, RN02 - RN04)
// ============================================================================

void abrirOrdemServico(const vector<Cliente>& clientes, const vector<Veiculo>& veiculos,
                         vector<OrdemServico>& ordens) {
    exibirCabecalho("ABRIR ORDEM DE SERVICO (RF04)");
    // RN02: Exige cliente e veiculo previamente cadastrados
    if (clientes.empty() || veiculos.empty()) {
        cout << "[ERRO - RN02] E necessario ter ao menos um cliente e um veiculo cadastrados.\n";
        return;
    }

    string cpf = lerLinha("CPF do cliente: ");
    int idxCli = buscarClientePorCPF(clientes, cpf);
    if (idxCli == -1) {
        cout << "[ERRO - RN02] Cliente nao cadastrado.\n";
        return;
    }

    string placa = paraMaiusculas(lerLinha("Placa do veiculo: "));
    int idxVei = buscarVeiculoPorPlaca(veiculos, placa);
    if (idxVei == -1) {
        cout << "[ERRO - RN02] Veiculo nao cadastrado.\n";
        return;
    }
    if (veiculos[idxVei].cpfCliente != cpf) {
        cout << "[ERRO - RN02] O veiculo " << placa << " pertence a outro cliente.\n";
        return;
    }

    OrdemServico os;
    os.id = proximoID_OS(ordens);
    os.cpfCliente = cpf;
    os.placaVeiculo = placa;
    os.dataEntrada = lerLinha("Data de entrada (dd/mm/aaaa): ");
    os.dataConclusao = "-";
    os.relatoFalha = lerLinha("Relato da falha / servico solicitado: ");
    os.status = "Em Analise"; // RF04: status inicial
    os.maoDeObra = 0.0;
    os.valorTotal = 0.0;

    ordens.push_back(os);
    salvarOrdens(ordens);

    cout << "\n[OK] Ordem de Servico #" << os.id << " aberta com status 'Em Analise'!\n";
}

void adicionarPecasEMaoDeObra(vector<OrdemServico>& ordens, vector<PecaItem>& pecas) {
    exibirCabecalho("ADICIONAR PECAS E MAO DE OBRA NA OS (RF05 / RF06)");
    int id = lerInteiro("Numero da Ordem de Servico (OS): ");
    int idx = buscarOSPorID(ordens, id);
    if (idx == -1) {
        cout << "[ERRO] Ordem de Servico #" << id << " nao encontrada.\n";
        return;
    }
    // RN04: Bloqueia alteracao de OS concluida
    if (ordens[idx].status == "Concluida") {
        cout << "[ERRO - RN04] A OS #" << id << " esta 'Concluida' e nao pode ter itens ou valores alterados.\n";
        return;
    }

    cout << "OS #" << id << " | Veiculo: " << ordens[idx].placaVeiculo
         << " | Status atual: " << ordens[idx].status << "\n\n";
    cout << "1 - Adicionar peca/item\n";
    cout << "2 - Definir/Atualizar valor da mao de obra\n";
    int op = lerInteiro("Escolha uma opcao: ");

    if (op == 1) {
        PecaItem p;
        p.idOS = id;
        p.nome = lerLinha("Nome da peca: ");
        p.valorUnitario = lerDouble("Valor unitario (R$): ");
        p.quantidade = lerInteiro("Quantidade: ");
        if (p.quantidade <= 0) {
            cout << "[ERRO] A quantidade deve ser maior que zero.\n";
            return;
        }
        pecas.push_back(p);
        recalcularTotalOS(ordens[idx], pecas);
        salvarPecas(pecas);
        salvarOrdens(ordens);
        cout << "\n[OK] Peca adicionada! Novo valor total da OS #" << id
             << ": R$ " << fixed << setprecision(2) << ordens[idx].valorTotal << "\n";
    } else if (op == 2) {
        ordens[idx].maoDeObra = lerDouble("Valor da mao de obra (R$): ");
        recalcularTotalOS(ordens[idx], pecas);
        salvarOrdens(ordens);
        cout << "\n[OK] Mao de obra atualizada! Novo valor total da OS #" << id
             << ": R$ " << fixed << setprecision(2) << ordens[idx].valorTotal << "\n";
    } else {
        cout << "[ERRO] Opcao invalida.\n";
    }
}

void atualizarStatusOS(vector<OrdemServico>& ordens) {
    exibirCabecalho("ATUALIZAR STATUS DA OS (RF07)");
    int id = lerInteiro("Numero da Ordem de Servico (OS): ");
    int idx = buscarOSPorID(ordens, id);
    if (idx == -1) {
        cout << "[ERRO] Ordem de Servico #" << id << " nao encontrada.\n";
        return;
    }
    if (ordens[idx].status == "Concluida") {
        cout << "[AVISO - RN04] A OS #" << id << " ja esta 'Concluida' e bloqueada para alteracoes.\n";
        return;
    }

    cout << "Status atual da OS #" << id << ": " << ordens[idx].status << "\n";
    cout << "Selecione o novo status:\n";
    cout << "1 - Em Analise\n";
    cout << "2 - Aprovada\n";
    cout << "3 - Em Manutencao\n";
    cout << "4 - Concluida\n";
    cout << "5 - Cancelada\n";
    int op = lerInteiro("Opcao: ");

    switch (op) {
        case 1: ordens[idx].status = "Em Analise"; break;
        case 2: ordens[idx].status = "Aprovada"; break;
        case 3: ordens[idx].status = "Em Manutencao"; break;
        case 4:
            ordens[idx].status = "Concluida";
            ordens[idx].dataConclusao = lerLinha("Data de conclusao (dd/mm/aaaa): ");
            // RN03: Garantia legal de 90 dias
            cout << "\n[GARANTIA - RN03] OS concluida em " << ordens[idx].dataConclusao
                 << ". Servico possui garantia legal de 90 dias a contar desta data.\n";
            break;
        case 5: ordens[idx].status = "Cancelada"; break;
        default:
            cout << "[ERRO] Opcao invalida.\n";
            return;
    }

    salvarOrdens(ordens);
    cout << "[OK] Status da OS #" << id << " atualizado para '" << ordens[idx].status << "'.\n";
}

void exibirDetalhesOS(const OrdemServico& os, const vector<Cliente>& clientes,
                      const vector<Veiculo>& veiculos, const vector<PecaItem>& pecas) {
    int idxCli = buscarClientePorCPF(clientes, os.cpfCliente);
    int idxVei = buscarVeiculoPorPlaca(veiculos, os.placaVeiculo);
    string nomeCli = (idxCli != -1) ? clientes[idxCli].nome : os.cpfCliente;
    string descVei = (idxVei != -1) ? (veiculos[idxVei].marca + " " + veiculos[idxVei].modelo) : "-";

    cout << "------------------------------------------------------------\n";
    cout << "OS #" << os.id << " | Status: " << os.status << "\n";
    cout << "Cliente:  " << nomeCli << " (CPF: " << os.cpfCliente << ")\n";
    cout << "Veiculo:  " << os.placaVeiculo << " - " << descVei << "\n";
    cout << "Entrada:  " << os.dataEntrada << " | Conclusao: " << os.dataConclusao << "\n";
    cout << "Relato:   " << os.relatoFalha << "\n";
    cout << "Pecas utilizadas:\n";
    bool temPeca = false;
    for (size_t i = 0; i < pecas.size(); ++i) {
        if (pecas[i].idOS == os.id) {
            temPeca = true;
            double sub = pecas[i].valorUnitario * pecas[i].quantidade;
            cout << "  - " << pecas[i].nome << " (Qtd: " << pecas[i].quantidade
                 << " x R$ " << fixed << setprecision(2) << pecas[i].valorUnitario
                 << ") = R$ " << sub << "\n";
        }
    }
    if (!temPeca) cout << "  (Nenhuma peca lancada)\n";
    cout << "Mao de Obra: R$ " << fixed << setprecision(2) << os.maoDeObra << "\n";
    cout << "VALOR TOTAL: R$ " << fixed << setprecision(2) << os.valorTotal << "\n";
    if (os.status == "Concluida") {
        cout << "Garantia:    90 dias a contar de " << os.dataConclusao << " (RN03)\n";
    }
}

void listarEPesquisarOS(const vector<OrdemServico>& ordens, const vector<Cliente>& clientes,
                        const vector<Veiculo>& veiculos, const vector<PecaItem>& pecas) {
    exibirCabecalho("LISTAR E PESQUISAR ORDENS DE SERVICO (RF08)");
    if (ordens.empty()) {
        cout << "Nenhuma Ordem de Servico registrada.\n";
        return;
    }

    cout << "1 - Listar OS abertas (nao concluidas/canceladas)\n";
    cout << "2 - Listar todas as OS\n";
    cout << "3 - Pesquisar por numero da OS\n";
    cout << "4 - Pesquisar por placa do veiculo\n";
    int op = lerInteiro("Escolha uma opcao: ");

    if (op == 1 || op == 2) {
        int cont = 0;
        for (size_t i = 0; i < ordens.size(); ++i) {
            if (op == 1 && (ordens[i].status == "Concluida" || ordens[i].status == "Cancelada")) {
                continue;
            }
            exibirDetalhesOS(ordens[i], clientes, veiculos, pecas);
            cont++;
        }
        if (cont == 0) cout << "\nNenhuma OS encontrada para este filtro.\n";
    } else if (op == 3) {
        int id = lerInteiro("Digite o numero da OS: ");
        int idx = buscarOSPorID(ordens, id);
        if (idx == -1) cout << "[AVISO] OS #" << id << " nao encontrada.\n";
        else exibirDetalhesOS(ordens[idx], clientes, veiculos, pecas);
    } else if (op == 4) {
        string placa = paraMaiusculas(lerLinha("Digite a placa do veiculo: "));
        int cont = 0;
        for (size_t i = 0; i < ordens.size(); ++i) {
            if (ordens[i].placaVeiculo == placa) {
                exibirDetalhesOS(ordens[i], clientes, veiculos, pecas);
                cont++;
            }
        }
        if (cont == 0) cout << "[AVISO] Nenhuma OS encontrada para a placa " << placa << ".\n";
    } else {
        cout << "[ERRO] Opcao invalida.\n";
    }
}

// ============================================================================
// MODULO DE RELATORIOS (RF09)
// ============================================================================

void gerarRelatorioAtendimentos(const vector<OrdemServico>& ordens,
                                const vector<Cliente>& clientes) {
    exibirCabecalho("RELATORIO DE ATENDIMENTOS E FATURAMENTO (RF09)");
    int totalConcluidas = 0;
    int totalAbertas = 0;
    int totalCanceladas = 0;
    double faturamentoTotal = 0.0;

    cout << "--- Servicos Concluidos ---\n";
    for (size_t i = 0; i < ordens.size(); ++i) {
        if (ordens[i].status == "Concluida") {
            totalConcluidas++;
            faturamentoTotal += ordens[i].valorTotal;
            int idxCli = buscarClientePorCPF(clientes, ordens[i].cpfCliente);
            string nomeCli = (idxCli != -1) ? clientes[idxCli].nome : ordens[i].cpfCliente;
            cout << "OS #" << ordens[i].id << " | Placa: " << ordens[i].placaVeiculo
                 << " | Cliente: " << nomeCli
                 << " | Concluida em: " << ordens[i].dataConclusao
                 << " | Total: R$ " << fixed << setprecision(2) << ordens[i].valorTotal << "\n";
        } else if (ordens[i].status == "Cancelada") {
            totalCanceladas++;
        } else {
            totalAbertas++;
        }
    }

    if (totalConcluidas == 0) {
        cout << "(Nenhum servico concluido ate o momento.)\n";
    }

    cout << "\n--- Resumo Geral ---\n";
    cout << "OS em andamento / abertas: " << totalAbertas << "\n";
    cout << "OS concluidas:             " << totalConcluidas << "\n";
    cout << "OS canceladas:             " << totalCanceladas << "\n";
    cout << "Faturamento Total Acumulado (OS Concluidas): R$ "
         << fixed << setprecision(2) << faturamentoTotal << "\n";
}

// ============================================================================
// CARGA DE DADOS REALISTAS PARA DEMONSTRACAO (OPCIONAL)
// ============================================================================

void carregarDadosDemonstracao(vector<Cliente>& clientes, vector<Veiculo>& veiculos,
                               vector<OrdemServico>& ordens, vector<PecaItem>& pecas) {
    if (!clientes.empty() || !ordens.empty()) {
        cout << "[AVISO] Ja existem dados salvos nos arquivos CSV. Carga cancelada para evitar duplicidade.\n";
        return;
    }

    clientes.push_back({"321.654.987-00", "Carlos Eduardo Mendes", "(11) 98765-4321", "Honda Civic 2018"});
    clientes.push_back({"456.789.123-11", "Fernanda Oliveira Costa", "(11) 99123-4567", "VW Polo 2020"});

    veiculos.push_back({"FGH4J56", "321.654.987-00", "Honda", "Civic LX", 2018});
    veiculos.push_back({"KLM8N90", "456.789.123-11", "Volkswagen", "Polo 1.6 MSI", 2020});

    OrdemServico os1 = {1, "321.654.987-00", "FGH4J56", "18/11/2026", "19/11/2026",
                        "Troca de oleo, filtros e revisao de freios", "Concluida", 220.00, 0.0};
    pecas.push_back({1, "Oleo Sintetico 5W30 (Litro)", 55.00, 4});
    pecas.push_back({1, "Filtro de Oleo", 45.00, 1});
    pecas.push_back({1, "Jogo de Pastilhas Dianteiras", 180.00, 1});
    recalcularTotalOS(os1, pecas);
    ordens.push_back(os1);

    OrdemServico os2 = {2, "456.789.123-11", "KLM8N90", "24/11/2026", "-",
                        "Barulho na suspensao dianteira e alinhamento", "Em Manutencao", 180.00, 0.0};
    pecas.push_back({2, "Bieleta Dianteira", 85.00, 2});
    recalcularTotalOS(os2, pecas);
    ordens.push_back(os2);

    salvarClientes(clientes);
    salvarVeiculos(veiculos);
    salvarPecas(pecas);
    salvarOrdens(ordens);

    cout << "[OK] Dados realistas de demonstracao salvos nos arquivos CSV!\n";
}

// ============================================================================
// FUNCAO PRINCIPAL (MENU INTERATIVO)
// ============================================================================

int main() {
    vector<Cliente> clientes;
    vector<Veiculo> veiculos;
    vector<OrdemServico> ordens;
    vector<PecaItem> pecas;

    carregarTodosDados(clientes, veiculos, ordens, pecas);

    int opcao = -1;
    while (opcao != 0) {
        exibirCabecalho("SISTEMA DE CONTROLE DE SERVICOS E CLIENTES");
        cout << "Registros carregados: " << clientes.size() << " clientes | "
             << veiculos.size() << " veiculos | " << ordens.size() << " OS\n";
        cout << "------------------------------------------------------------\n";
        cout << "1 - Cadastrar Cliente (RF01)\n";
        cout << "2 - Consultar / Listar Clientes (RF02)\n";
        cout << "3 - Cadastrar Veiculo (RF03)\n";
        cout << "4 - Listar Veiculos\n";
        cout << "5 - Abrir Ordem de Servico - OS (RF04)\n";
        cout << "6 - Adicionar Pecas e Mao de Obra na OS (RF05 / RF06)\n";
        cout << "7 - Atualizar Status da OS (RF07)\n";
        cout << "8 - Listar e Pesquisar OS (RF08)\n";
        cout << "9 - Relatorio de Atendimentos e Faturamento (RF09)\n";
        cout << "10 - Carregar Dados de Exemplo (Demonstracao)\n";
        cout << "0 - Sair do Sistema\n";
        cout << "------------------------------------------------------------\n";

        opcao = lerInteiro("Digite a opcao desejada: ");

        switch (opcao) {
            case 1: cadastrarCliente(clientes); pausar(); break;
            case 2: consultarOuListarClientes(clientes); pausar(); break;
            case 3: cadastrarVeiculo(clientes, veiculos); pausar(); break;
            case 4: listarVeiculos(veiculos, clientes); pausar(); break;
            case 5: abrirOrdemServico(clientes, veiculos, ordens); pausar(); break;
            case 6: adicionarPecasEMaoDeObra(ordens, pecas); pausar(); break;
            case 7: atualizarStatusOS(ordens); pausar(); break;
            case 8: listarEPesquisarOS(ordens, clientes, veiculos, pecas); pausar(); break;
            case 9: gerarRelatorioAtendimentos(ordens, clientes); pausar(); break;
            case 10: carregarDadosDemonstracao(clientes, veiculos, ordens, pecas); pausar(); break;
            case 0:
                cout << "\nEncerrando o sistema Ortega Car. Todos os dados estao salvos em CSV.\n";
                break;
            default:
                cout << "[ERRO] Opcao invalida. Escolha entre 0 e 10.\n";
                pausar();
                break;
        }
    }

    return 0;
}
