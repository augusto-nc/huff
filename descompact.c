#include "descompact.h"
#include "three.h"
#include "help.h"

Node* recreate_tree(Node* root, byte *data, int *pos, int size_tree){

    if((*pos) < size_tree){
        if(root == NULL){
            root = insert(bt,data[(*pos)]);
            (*pos)++;
        }

        if(data[(*pos)] == asterisco){
            root->left = recreate_tree(root->left,data,pos);
            root->right = recreate_tree(root->right,data,pos);
        }else
            (*pos)++;
    }

    return root;
}



void calcule_size_file(byte *data, int pos, Node* root, long size_of_file_compacted, long* size_of_file_descompacted, int trash){
    Node* current = root;
    int i,j;
    for( i=(*pos) ; i<size_of_file_compacted ; i++){
        if(i == size_of_file_compacted - 1){
            for(j=trash ; j<8 ; j++){
                int way = is_bit_i_set( data[i] , j)
                if(current != NULL ){
                    if(way == 0)
                        current = current->left;
                    else
                        current = current->right;
                }
                else{
                    (*size_of_file_descompacted)++;
                    current = root;
                }
            }
        }

        else{
            for(j=0 ; j<8 ; j++){
                int way = is_bit_i_set( data[i] , j)
                if(current != NULL ){
                    if(way == 0)
                        current = current->left;
                    else
                        current = current->right;
                }
                else{
                    (*size_of_file_descompacted)++;
                    current = root;
                }
            }
        }
    }
}

byte* Rewrite(byte *data, int pos , Node* root , long size_of_file_compacted, long* size_of_file_descompacted, int trash ){
    Node* current = root;
    int i,j;

    byte* descompac_file = (byte*) malloc(sizeof(byte)*(*size_of_file_descompacted));
    int loc = 0;

    for( i=(*pos) ; i<size_of_file_compacted ; i++){
        if((*pos) == size_of_file_compacted - 1){
            for(j=trash ; j<8; j++){
                int way = is_bit_i_set( data[i] , j)

                if(current->left != NULL ){
                    if(way == 0)
                        current = current->left;
                    else
                        current = current->right;
                }

                else{
                    descompac_file[loc++] = current->valor;
                    current = root;
                }
            }
        }
        else{
            for(j=0 ; j<8 - trash ; j++){
                int way = is_bit_i_set( data[i] , j)

                if(current->left != NULL ){
                    if(way == 0)
                        current = current->left;
                    else
                        current = current->right;
                }

                else{
                    descompac_file[loc++] = current->valor;
                    current = root;
                }
            }
        }
    }

    return descompac_file;

}

byte* descompact(byte* data, long size_of_file_compacted, long* size_of_file_descompacted){

    byte trash = data[0]>>5;
    int trash_size = (int) trash;

    Double_byte tree_s = data[0];
    tree_s = tree_s<<11;
    tree_s = tree_s>>3
    tree_s = tree_s | data[1];

    int size_tree = (int) tree_s;

    Node *root;
    int index = 2;

    root = recreate_tree(root, data, &index, size_tree);

    calcule_size_file(data, index, root, size_of_file_compacted, &size_of_file_descompacted);

    byte *output;
    output = Rewrite(data, index, root, size_of_file_compacted);

    return output;
}

