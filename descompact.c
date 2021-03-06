#include "descompact.h"

Node* recreate_tree(Node* root, byte *data, int *pos, int size_tree){       //Pega os dados do cabealho para recriar a arvore

    if((*pos) < size_tree+2){       // size_tree + 2 porque vai até o tamanho da arvore +2, pois começa do indice 2

        if(root == NULL){           // Se a raíz for vazia, cria um nó e coloca o valor da posição no arquivo nele.
            root = createNode();
            root->valor=data[(*pos)];
        }

        if(data[(*pos)] == asterisco){                                  // Se for um asterisco,
           (*pos)++;                                                    // Acrescenta 1 na posição para ver o próximo elemento
            root->left = recreate_tree(root->left,data,pos,size_tree);  // E chama recursivamente a função para preencher o lado esquerdo
            root->right = recreate_tree(root->right,data,pos,size_tree);// analogamente o lado direito
        }else if(data[(*pos)] == escape){                              // Caso seja um escape "\"
            (*pos)++;                                                   // Adiciona 1 na posição para ver o próximo elemento
            root->valor=data[(*pos)++];                                 // Coloca o elemento no nó e acrescenta na posição (para ir para o valor seguinte)
        }else{
           (*pos)++;                                                    // Caso não seja asterisco nem escape, deixa o elemento no node e vai pra proxima posição
        }
    }

    return root;                                                        // Retorna a referência para a Raiz da arvore.
}



void calcule_size_file(byte *data, int pos, Node* root, long size_of_file_compacted, long* size_of_file_descompacted, int trash){       //Calcula o tamanho que o arquivo ficará após a descompactação
    Node* current = root;           //Cria uma ponteiro auxiliar e o põe para aponta para a raiz da arvore
    int i,j,trashAux;
    trashAux=0;
    for( i=pos ; i<size_of_file_compacted ; i++){   //Vai da posição do final da arvore do cabeçalho até o final do arquivo
            if(i == size_of_file_compacted - 1){    // Se estivermos vendo o último byte do arquivo, desconsideraremos o lixo,
                   trashAux=trash;
            }
            for(j=0 ; j<8-trashAux ; j++){
              int way = is_bit_i_set( data[i] , j);  // variavel auxiliar recebe informação do bit do arquivo, para saber que caminho toma na arvore
                    if(way == 0)
                        current = current->left;      // Se for 0, pega o caminho da esquerda
                    else
                        current = current->right;      // se for 1, pega o caminho da direita

                if(current->left == NULL ){           //Caso chegue em uma folha,
                    (*size_of_file_descompacted)++;    // Adicionamos 1byte no tamanho do arquivo
                    current = root;                     // e voltamos nosso ponteiro auxiliar para o começo da arvore.
                }
            }
    }
}

byte* rewrite(byte *data, int pos , Node* root , long size_of_file_compacted, long* size_of_file_descompacted, int trash ){ //Reescreve a informação no arquivo
    Node* current = root;
    int i,j;

    byte* descompac_file = (byte*) malloc(sizeof(byte)*(*size_of_file_descompacted));   //Alloca memória do tamanho do arquivo descompactado
    int loc = 0;
    int trasAux=0;
    for( i=pos ; i<size_of_file_compacted ; i++){
        if(pos == size_of_file_compacted - 1){      //Caso estejamos no último byte, desconsideraremos o lixo
            trasAux=trash;
        }
            for(j=0 ; j<8-trasAux; j++){
                int way = is_bit_i_set( data[i] , j);
                if(way == 0)
                        current = current->left;
                    else
                        current = current->right;

                if(current->left == NULL ){         //Caso esteja em uma folha

                    descompac_file[loc++] = current->valor; //Guardamos o valor da folha no arquivo e acrescentamos 1 na posição, para não sobreescrever
                    current = root;                         // volta para o início da arvore, para refazer a analise
                }
            }
    }

    return descompac_file;

}

byte* descompact(byte* data, long size_of_file_compacted, long* size_of_file_descompacted){         //Descompacta o arquivo

    byte trash = data[0]>>5;            //data[0] os 3 primeiros bits são o tamanho do lixo
    int trash_size = (int) trash;       // pega o tamanho do lixo

    double_byte tree_s = data[0];       // os 5 próximos bits de data[0] compõem juntamente com o data[1] o tamanho da arvore
    tree_s = tree_s<<11;
    tree_s = tree_s>>3;
    tree_s = tree_s | data[1];

    int size_tree = (int) tree_s;      // pega o tamanho da arvore

    Node *root=NULL;
    int index = 2;                      // indice 2 porque queremos pegar o primeiro elemento da arvore no cabeçalho.

    root = recreate_tree(root, data, &index, size_tree);        // Recria a arvore

    calcule_size_file(data, index, root, size_of_file_compacted, size_of_file_descompacted,trash);  // Calcula o tamanho do arquivo original (descompactado)
    // int size=*size_of_file_descompacted;
    byte *output;
    output = rewrite(data, index, root, size_of_file_compacted,size_of_file_descompacted,trash);    // Reescreve o arquivo

    return output;
}

