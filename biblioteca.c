#include <stdio.h>
#include <stdlib.h>
#include <string.h>
///////////////////////////////////////////////////////////////////////////////////////////////////////////q
typedef struct biblioteca Biblioteca;
typedef struct livro Livro;
typedef struct pilha Pilha;
typedef struct fila Fila;
typedef struct pedido Pedido;
typedef struct camada Camada;

Biblioteca *biblioteca_cria(void);
void biblioteca_imprime(Biblioteca *biblioteca);
void biblioteca_libera(Biblioteca *biblioteca);

Livro *livro_cria(int ano, char *autor, char *titulo, int qntd_exemplares);
void livro_adiciona_ordenado(Biblioteca *biblioteca, Livro *livro);
void livro_empresta(Biblioteca *biblioteca, int matricula_aluno, char *titulo_empresta);
void livro_devolve(Biblioteca *biblioteca, int matricula_aluno, char *titulo_devolve);
void livro_imprime(Livro *livro);
void livro_libera(Livro *livro);

Pilha *pilha_cria(int qntd_camadas);
void pilha_push(Livro *livro, Pilha *pilha);
void pilha_pop(Pilha *pilha);
int pilha_vazia(Pilha *pilha);
int pilha_cheia(Pilha *pilha, int qntd_exemplares);
void pilha_imprime(Pilha *pilha);
void pilha_libera(Pilha *pilha);

Camada *camada_cria(void);
void camada_libera(Camada *camada);

Fila *fila_cria(void);
void fila_adiciona_pedido(Fila *fila, Pedido *pedido);
void fila_retira_pedido(Fila *fila);
void fila_imprime(Fila *fila);
void fila_libera(Fila *fila);
int fila_vazia(Fila *fila);

Pedido *pedido_cria(int n_mat);
void pedido_libera(Pedido *pedido);
//////////////////////////////q/////////////////////////////////////////////////////////////////////////////q
// algumas observações que podem ser melhoradas, mas que não quebram o programa
// 1 - caso o usuario esqueca de cadastrar o ano de publicao do livro o codigo assume o ultimo ano inserido como no exemplo:
//3
//2019
//Eu 
//teste//
//1
//2018
//eu
//teste2
//1
//eu        (usuario nao digita o ano)
//teste3
//1
//2018
//eu
//teste3
//1
//fila vazia
//2018
//eu
//teste2
//1
//fila vazia
//2019
//Eu
//teste
//1
//fila vazia
// 2 - caso seja cadastrado apenas um livro, mas o usuario esquece de de inserir o ano, o codigo assume 'ano = 0'
//1
//eu        (usuario nao digita o ano)
//teste
//1
//0
//eu
//teste
//1
//fila vazia
//  3 - o codigo nao executa operacao de emprestimo ou devolucao caso o num de matricula tenha mais de 4 digitos*
//  21657*
//  teste
//  E
//  0
//  eu
//  teste
//  1(não emprestou... nao devolve tambem, acredite)
//  fila vazia
// 4 - caso o usuario esqueca a matricula na hora de emprestar o livro, o codigo assume "num_mat = 0"
//1
//2019
//eu
//teste
//1
//teste (usuario nao digita o num_mat)
//E
//teste (usuario nao digita o num_mat)
//E
//2019
//eu
//teste
//pilha vazia
//0 (numero de matricula do ultimo pedido de emprestimo)
int main(void)
{
    int qntd_livros, ano_livro, qntd_exemplares, num_mat, i;
    char autor[100], titulo[100], op;
    Biblioteca *biblioteca = biblioteca_cria();

    scanf("%d", &qntd_livros);
    for (i = 1; i <= qntd_livros; i++)
    {
        scanf("%d", &ano_livro);
        scanf("%100s", autor);
        scanf("%100s", titulo);
        scanf("%d", &qntd_exemplares);
        Livro *livro = livro_cria(ano_livro, autor, titulo, qntd_exemplares);
        livro_adiciona_ordenado(biblioteca, livro);
    }
    while (scanf("%4d", &num_mat) != EOF)
    {
        fflush(stdin);
        scanf("%100s", titulo);
        fflush(stdin);
        scanf(" %c", &op);
        switch (op)
        {
        case 'E':
            livro_empresta(biblioteca, num_mat, titulo);
            break;

        case 'D':
            livro_devolve(biblioteca, num_mat, titulo);
            break;
        default:
            break;
        }
    }
    biblioteca_imprime(biblioteca);
    biblioteca_libera(biblioteca);
    return 0;
}

/////////////////////////////////////////////////////////////////////////////q/////////////////////////////////////

struct biblioteca
{
    Livro *primeiro;
};

struct livro
{
    int ano_public;
    char autor[101];
    char titulo[101];
    int qntd_exemplares;
    Pilha *pilha_exemplares;
    Fila *fila_espera;
    Livro *proximo;
};
//O tipo de pilha usado no livro é implementado como lista encadeada
struct pilha
{
    int qntd_atual;
    Camada *topo;
};
//O tipo Camada é manipulado na pilha, apenas pra constar que foi feito o manuseio de maneira correta.
struct camada
{
    int numero_livro;
    Camada *camada_abaixo;
};

// cada livro possui uma fila de espera. Para facilitar a manipulação da estrutura,
// tornar o codigo menos custoso e mais eficiente, utilizo um ponteiro para indicar
// o inicio e um ponteiro para indicar o fim
struct fila
{
    int num_pedidos;
    Pedido *primeiro, *ultimo;
};
// a estrutura pedido é a reserva que entra na fila de espera
struct pedido
{
    int matricula_aluno;
    Pedido *proximo_fila;
};

//A função retorna uma biblioteca vazia.
Biblioteca *biblioteca_cria(void)
{
    Biblioteca *biblioteca = (Biblioteca *)malloc(sizeof(biblioteca));
    if (biblioteca == NULL)
    {
        printf("Memoria insuficiente. ");
        exit(1);
    }
    biblioteca->primeiro = NULL;
    return biblioteca;
}
void biblioteca_imprime(Biblioteca *biblioteca)
{
    if (biblioteca->primeiro != NULL)
    {
        Livro *livro_atual = biblioteca->primeiro;
        for (livro_atual; livro_atual != NULL; livro_atual = livro_atual->proximo)
        {
            livro_imprime(livro_atual);
        }
    }
}
// Nome auto explativo
void biblioteca_libera(Biblioteca *biblioteca)
{
    Livro *livro = biblioteca->primeiro;
    if (livro != NULL)
    {
        // percorre os livros da biblioteca e libera livro por livro
        for (livro; livro != NULL; livro = biblioteca->primeiro)
        {
            biblioteca->primeiro = livro->proximo;
            livro_libera(livro);
        }
        free(biblioteca);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////////qqq/////////////////////////
Livro *livro_cria(int ano, char *nome_autor, char *livro_titulo, int qntd_exemplares)
{
    Livro *novo_livro = (Livro *)malloc(sizeof(Livro));
    if (novo_livro == NULL)
    {
        printf("Memoria insuficiente. ");
        exit(1);
    }

    novo_livro->ano_public = ano;
    strcpy(novo_livro->autor, nome_autor);
    strcpy(novo_livro->titulo, livro_titulo);
    novo_livro->qntd_exemplares = qntd_exemplares;
    novo_livro->pilha_exemplares = pilha_cria(qntd_exemplares);
    novo_livro->fila_espera = fila_cria();
    //printf("\nLivro criado\n\n");
    return novo_livro;
}
void livro_adiciona_ordenado(Biblioteca *biblioteca, Livro *livro)
{
    Livro *livro_atual = biblioteca->primeiro;
    Livro *livro_anterior = NULL;
    //percorre a biblioteca ate encontrar o livro com ano de publicacao menor ou o fim da biblio
    while (livro_atual != NULL && livro_atual->ano_public < livro->ano_public)
    {
        livro_anterior = livro_atual;
        livro_atual = livro_atual->proximo;
    }
    //verifica se a biblioteca esta vazia
    if (livro_anterior == NULL)
    {
        //nesse caso o livro é inserido no inicio da biblioteca
        livro->proximo = biblioteca->primeiro;
        biblioteca->primeiro = livro;
    }
    else
    {
        //nesse caso o livro é adicionado na sequencia crescente da biblioteca,
        // o codigo interpreta anos iguais como maior,
        // sendo assim livros do mesmo ano serão ordenados pela ordem de digitacao
        // ex: adicionando 2 livros
        // 2
        //ano: 2019
        //autor: nome1
        //titulo: teste1
        //qntd_de_exemplares: 1
        //ano: 2019
        //autor: nome2
        //titulo: teste2
        //qntd_de_exemplares: 1
        //EOF(Ctrl+D) para indicar o fim das operacoes, sem emprestar ou devolver livros
        // a impressao da biblioteca deve mostrar assim:
        //2019
        //nome2
        //titulo2
        //1
        //fila vazia
        //2019
        //nome1
        //titulo1
        //1
        //fila vazia
        livro->proximo = livro_anterior->proximo;
        livro_anterior->proximo = livro;
    }
}
void livro_empresta(Biblioteca *biblioteca, int matricula_aluno, char *titulo_empresta)
{
    Livro *livro_atual = biblioteca->primeiro;
    //Percorre a biblioteca até encontrar o livro
    while (livro_atual != NULL)
    { //compara o titulos dos livro da biblioteca(livro_atual->titulo) com o titulo informado pelo usuario(titulo_empresta)
        if (!strcmp(livro_atual->titulo, titulo_empresta))
        {
            break;
        }
        livro_atual = livro_atual->proximo;
    }

    if (livro_atual != NULL)
    {
        //Verifica se a pilha de exemplares esta vazia ou se tem fila de espera
        if (pilha_vazia(livro_atual->pilha_exemplares) || !fila_vazia(livro_atual->fila_espera))
        {
            Pedido *novo_pedido = malloc(sizeof(Pedido));
            if (novo_pedido == NULL)
            {
                printf("Memoria insuficiente. ");
                exit(1);
            }
            novo_pedido->matricula_aluno = matricula_aluno;
            //adiciona o pedido na fila de espera
            fila_adiciona_pedido(livro_atual->fila_espera, novo_pedido);
        }
        else
        {
            //retiro um exemplar da pilha
            pilha_pop(livro_atual->pilha_exemplares);
        }
    }
}
void livro_devolve(Biblioteca *biblioteca, int matricula_aluno, char *titulo_devolve)
{
    Livro *livro_atual = biblioteca->primeiro;
    //Percorre a biblioteca até encontrar o livro
    while (livro_atual != NULL)
    {
        if (!strcmp(livro_atual->titulo, titulo_devolve))
        {
            //aqui uma gamb de leve pra interromper a busca linear...
            break;
        }
        livro_atual = livro_atual->proximo;
    }
    // Vou checkar se ainda tem livro a ser devolvido, ou seja, se a pilha de exemplares esta incompleta,
    // nao cheia, como na condicao 'if'
    if (livro_atual != NULL)
    {
        if (!pilha_cheia(livro_atual->pilha_exemplares, livro_atual->qntd_exemplares))
        {
            //checko se tem fila de espera
            if (!fila_vazia(livro_atual->fila_espera))
            {
                // Retiro um pedido da fila, e pera... testando...
                fila_retira_pedido(livro_atual->fila_espera);
                // Isso, eu apena retiro o pedido da fila de espera, sem da o
                //pilha_push(livro_atual, livro_atual->pilha_exemplares), como tava antes.
            }
            else
            {
                //Apenas devolvo o exemplar para a pilha, pois nao tem pedido na fila de espera
                pilha_push(livro_atual, livro_atual->pilha_exemplares);
            }
        }
    }
}
void livro_libera(Livro *livro)
{
    if (livro != NULL)
    {
        //libera as estruturas dentro do livro e por fim o livro
        pilha_libera(livro->pilha_exemplares);
        fila_libera(livro->fila_espera);
        free(livro);
    }
}
// Bom... essa funcao é "óvibia"
void livro_imprime(Livro *livro)
{
    printf("%d\n", livro->ano_public);
    printf("%s\n", livro->autor);
    printf("%s\n", livro->titulo);
    pilha_imprime(livro->pilha_exemplares);
    fila_imprime(livro->fila_espera);
}
///////////////////////////////////////////////////////////////////////////////////////////////////q¹///////////////
Pilha *pilha_cria(int qntd_camadas)
{
    Pilha *nova_pilha = (Pilha *)malloc(sizeof(Pilha));
    if (nova_pilha == NULL)
    {
        printf("Memoria insuficiente");
        exit(1);
    }
    //é interessante salientar que a pilha de exemplares disponiveis se inicia cheia
    nova_pilha->qntd_atual = qntd_camadas;
    int i;
    //aqui eu "preencho a pilha" com a qntd inicial de exemplares disponiveis,
    // mas elas são só pra constar que eu trato a lista com o conceito de pilha mesmo,
    // pois eu uso a 'pilha->qntd_atual' da estrutura pra informar o estado na hora da impressao
    for (i = 1; i <= qntd_camadas; i++)
    {
        Camada *c = camada_cria();
        c->numero_livro = i;
        c->camada_abaixo = nova_pilha->topo;
        nova_pilha->topo = c;
    }
    return nova_pilha;
}
void pilha_pop(Pilha *pilha)
{
    if (!pilha_vazia(pilha))
    {
        //se a pilha nao tiver vazia
        // a 'c_libera' recebe o topo que sera liberado, pra obedecer o conceito de pilha
        Camada *c_libera = pilha->topo;
        //o topo aponta pra camada mediatamente abaixo do atual topo
        pilha->topo = c_libera->camada_abaixo;
        //decrementa a quantidade de exemplares disponiveis na pilha
        pilha->qntd_atual = pilha->qntd_atual - 1;
        // e libera a camada
        camada_libera(c_libera);
    }
}
void pilha_push(Livro *livro, Pilha *pilha)
{
    // so adiciona um novo exemplar na no top da pilha se ela não esta "cheia"
    if (!pilha_cheia(pilha, livro->qntd_exemplares))
    {
        pilha->qntd_atual = pilha->qntd_atual + 1;

        Camada *nova_camada = camada_cria();
        // referencia o antigo topo da pilha como a camada imediatamente abaixo
        nova_camada->camada_abaixo = pilha->topo;
        // verifica se e o primeiro elemento adiconado na pilha, o elemento que fica lá na base da pilha
        if (nova_camada->camada_abaixo == NULL)
        {
            // esse atributo 'numero_livro' da estrutura é só enfeite, eu poderia ter aproveitado melhor,
            // só não vou mexer porque o codigo já tá rodando
            nova_camada->numero_livro = 1;
        }
        else
        {
            nova_camada->numero_livro = nova_camada->camada_abaixo->numero_livro + 1;
        }
        pilha->topo = nova_camada;
    }
}
void pilha_libera(Pilha *pilha)
{
    if (!pilha_vazia(pilha))
    {
        Camada *c;
        //percore a pilha desde o topo e vai liberando camada por camada
        for (c = pilha->topo; c != NULL; c = pilha->topo)
        {
            pilha->topo = c->camada_abaixo;
            camada_libera(c);
        }
        // libera a pilha de exemplares do livro
        free(pilha);
    }
}
int pilha_cheia(Pilha *pilha, int qntd_exemplares)
{
    // pilha cheia retorna 1, nao cheia retorna 0
    if (pilha->qntd_atual < qntd_exemplares)
    {
        return 0;
    }
    return 1;
}
int pilha_vazia(Pilha *pilha)
{
    // pilha vazia retorna 1, nao vazia retorna 0
    if (pilha->topo == NULL)
    {
        return 1;
    }
    return 0;
}
void pilha_imprime(Pilha *pilha)
{
    //como comentado em algum lugar do codigo
    // eu uso a 'qntd_atual' pra idicar o estado da pilha quando imprimo a biblioteca
    if (pilha->qntd_atual > 0)
    {
        printf("%d\n", pilha->qntd_atual);
    }
    else
    {
        printf("pilha vazia\n");
    }
}
Camada *camada_cria(void)
{
    Camada *c = (Camada *)malloc(sizeof(Camada));
    if (c == NULL)
    {
        printf("Memoria insuficiente.");
        exit(1);
    }
    return c;
}
void camada_libera(Camada *camada)
{
    if (camada != NULL)
    {
        free(camada);
    }
}
////////////////////////////////////////////////////////////////////////////////////////////////////////qqq
Fila *fila_cria(void)
{
    Fila *fila = (Fila *)malloc(sizeof(Fila));
    if (fila == NULL)
    {
        printf("Memoria insuficiente.");
        exit(1);
    }
    // o estado incial de toda fila de espera é vazio
    fila->primeiro = NULL;
    fila->ultimo = NULL;
    fila->num_pedidos = 0;

    return fila;
}
void fila_adiciona_pedido(Fila *fila, Pedido *pedido)
{
    pedido->proximo_fila = NULL;
    //obedecendo o conceito fifo, first-in,
    if (fila_vazia(fila))
    {
        fila->primeiro = pedido;
        fila->ultimo = pedido;
    }
    else
    {
        fila->ultimo->proximo_fila = pedido;
        fila->ultimo = pedido;
    }
}
void fila_retira_pedido(Fila *fila)
{
    // first-out
    if (!fila_vazia(fila))
    {
        Pedido *p = fila->primeiro;
        fila->primeiro = fila->primeiro->proximo_fila;
        p->proximo_fila = NULL;
        free(p);
    }
}
void fila_libera(Fila *fila)
{
    if (!fila_vazia(fila))
    {
        Pedido *p;
        for (p = fila->primeiro; p != NULL; p = fila->primeiro)
        {
            fila->primeiro = fila->primeiro->proximo_fila;
            p->proximo_fila = NULL;
            free(p);
        }
        free(fila);
    }
}
int fila_vazia(Fila *fila)
{
    if (fila->primeiro == NULL)
    {
        return 1;
    }
    return 0;
}
void fila_imprime(Fila *fila)
{
    Pedido *p = fila->primeiro;
    //percorro a fila caso tenha reserva e imprimo as matriculas
    for (p; p != NULL; p = p->proximo_fila)
    {
        // aqui apenas um condicao pra cologar as virgulas(, ) corretamente
        if (p->proximo_fila == NULL)
        {
            printf("%d\n", p->matricula_aluno);
        }
        else
        {
            printf("%d, ", p->matricula_aluno);
        }
    }
    if (fila_vazia(fila))
    {
        printf("fila vazia\n");
    }
}
Pedido *pedido_cria(int n_mat)
{
    Pedido *pedido = (Pedido *)malloc(sizeof(Pedido));
    if (pedido == NULL)
    {
        printf("Memoria insuficiente.");
        exit(1);
    }
    pedido->proximo_fila = NULL;
    pedido->matricula_aluno = n_mat;
    return pedido;
}
void pedido_libera(Pedido *pedido)
{
    free(pedido);
}