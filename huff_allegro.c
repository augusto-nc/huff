#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>

int main(){
// VARÍAVEIS DA INTERFACE
    const int LARGURA_TELA = 650;
    const int ALTURA_TELA = 450;

    bool fim = false;
    ALLEGRO_EVENT_QUEUE *fila_eventos = NULL;
// INICIALIZAÇÃO DA ALLEGRO E DO DISPLAY
    ALLEGRO_DISPLAY *display = NULL;
    if(!al_init())
    {
        al_show_native_message_box(NULL,"ALERTA!","ERRO", "ERRO AO INICIAR O ALLEGRO",NULL,ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }
    display = al_create_display(LARGURA_TELA,ALTURA_TELA);

    if(!display)
    {
        al_show_native_message_box(NULL,"ALERTA!","ERRO", "ERRO AO ABRIR O DISPLAY",NULL,ALLEGRO_MESSAGEBOX_ERROR);
        return -1;
    }

// INICIALIZAÇÃO DE ADDONS E INSTALAÇÕES
    al_install_keyboard();

// CRIAÇÃO DA FILA DE EVENTOS E DEMAIS DISPOSITIVOS
    fila_eventos = al_create_event_queue();

// REGISTRO DE SOURCES
    al_register_event_source(fila_eventos,al_get_keyboard_event_source());
    al_register_event_source(fila_eventos,al_get_display_event_source(display));

// LOOP PRINCIPAL
    while(!fim){
        ALLEGRO_EVENT ev;

        al_wait_for_event(fila_eventos, &ev);

        if(ev.type == ALLEGRO_EVENT_KEY_DOWN)
            {
                if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                {
                    fim = true;
                }

        }
        if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
            fim = true;
        }
        al_flip_display();
    }
// FINALIZAÇÕES DO PROGRAMA
    al_destroy_display(display);
    al_destroy_event_queue(fila_eventos);


return 0;
}
