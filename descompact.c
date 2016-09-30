#include "descompact.h"

Node* recreate_tree(Node* root, byte *data, int *pos, int size_tree){

    if((*pos) < size_tree+2){

        if(root == NULL){
            root = createNode();
            root->valor=data[(*pos)];
        }

        if(data[(*pos)] == asterisco){
           (*pos)++;
            root->left = recreate_tree(root->left,data,pos,size_tree);
            root->right = recreate_tree(root->right,data,pos,size_tree);
        }else if(data[(*pos)] == escape){
            (*pos)++;
            root->valor=data[(*pos)++];
        }else{
           (*pos)++;
        }
    }

    return root;
}



void calcule_size_file(byte *data, int pos, Node* root, long size_of_file_compacted, long* size_of_file_descompacted, int trash){
    Node* current = root;
    int i,j,trashAux;
    trashAux=0;
    for( i=pos ; i<size_of_file_compacted ; i++){
            if(i == size_of_file_compacted - 1){
                   trashAux=trash;
            }
            for(j=0 ; j<8-trashAux ; j++){
              int way = is_bit_i_set( data[i] , j);
                    if(way == 0)
                        current = current->left;
                    else
                        current = current->right;

                if(current->left == NULL ){
                    (*size_of_file_descompacted)++;
                    current = root;
                }
            }
    }
}


byte* rewrite(byte *data, int pos , Node* root , long size_of_file_compacted, long* size_of_file_descompacted, int trash ){
    Node* current = root;
    int i,j;

    byte* descompac_file = (byte*) malloc(sizeof(byte)*(*size_of_file_descompacted));
    int loc = 0;
    int trasAux=0;
    for( i=pos ; i<size_of_file_compacted ; i++){
        if(pos == size_of_file_compacted - 1){
            trasAux=trash;
        }
            for(j=0 ; j<8-trasAux; j++){
                int way = is_bit_i_set( data[i] , j);
                if(way == 0)
                        current = current->left;
                    else
                        current = current->right;

                if(current->left == NULL ){

                    descompac_file[loc++] = current->valor;
                    current = root;
                }
            }
    }

    return descompac_file;

}

byte* descompact(byte* data, long size_of_file_compacted, long* size_of_file_descompacted){

    byte trash = data[0]>>5;
    int trash_size = (int) trash;

    double_byte tree_s = data[0];
    tree_s = tree_s<<11;
    tree_s = tree_s>>3;
    tree_s = tree_s | data[1];

    int size_tree = (int) tree_s;

    Node *root=NULL;
    int index = 2;

    root = recreate_tree(root, data, &index, size_tree);

    calcule_size_file(data, index, root, size_of_file_compacted, size_of_file_descompacted,trash);
    int size=*size_of_file_descompacted;
    byte *output;
    output = rewrite(data, index, root, size_of_file_compacted,size_of_file_descompacted,trash);

    return output;
}

