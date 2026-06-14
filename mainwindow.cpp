#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    sistema.cargarMapaInicial();

    // =====================================
    // MENU CIUDAD ORIGEN
    // =====================================

    QMenu *ciudadOrigen = new QMenu(this);

    ciudadOrigen->addAction("Cordoba Capital");
    ciudadOrigen->addAction("Villa Carlos Paz");
    ciudadOrigen->addAction("Villa Maria");
    ciudadOrigen->addAction("Rio Cuarto");
    ciudadOrigen->addAction("Bell Ville");
    ciudadOrigen->addAction("San Francisco");

    ui->ButtonCiudadOrigen->setMenu(ciudadOrigen);
    ui->ButtonCiudadOrigen->setPopupMode(QToolButton::InstantPopup);

    connect(ciudadOrigen, &QMenu::triggered,
            this,
            [=](QAction *accion)
            {
                ui->ButtonCiudadOrigen->setText(accion->text());

                if(accion->text() == "Cordoba Capital")
                    origenSeleccionado = 0;
                else if(accion->text() == "Villa Carlos Paz")
                    origenSeleccionado = 1;
                else if(accion->text() == "Villa Maria")
                    origenSeleccionado = 2;
                else if(accion->text() == "Rio Cuarto")
                    origenSeleccionado = 3;
                else if(accion->text() == "Bell Ville")
                    origenSeleccionado = 4;
                else if(accion->text() == "San Francisco")
                    origenSeleccionado = 5;
            });





    // =====================================
    // MENU CIUDAD DESTINO
    // =====================================

    QMenu *ciudadDestino = new QMenu(this);

    ciudadDestino->addAction("Cordoba Capital");
    ciudadDestino->addAction("Villa Carlos Paz");
    ciudadDestino->addAction("Villa Maria");
    ciudadDestino->addAction("Rio Cuarto");
    ciudadDestino->addAction("Bell Ville");
    ciudadDestino->addAction("San Francisco");

    ui->ButtonCiudadDestino->setMenu(ciudadDestino);
    ui->ButtonCiudadDestino->setPopupMode(QToolButton::InstantPopup);


    connect(ciudadDestino, &QMenu::triggered,
            this,
            [=](QAction *accion)
            {
                ui->ButtonCiudadDestino->setText(accion->text());

                if(accion->text() == "Cordoba Capital")
                    destinoSeleccionado = 0;
                else if(accion->text() == "Villa Carlos Paz")
                    destinoSeleccionado = 1;
                else if(accion->text() == "Villa Maria")
                    destinoSeleccionado = 2;
                else if(accion->text() == "Rio Cuarto")
                    destinoSeleccionado = 3;
                else if(accion->text() == "Bell Ville")
                    destinoSeleccionado = 4;
                else if(accion->text() == "San Francisco")
                    destinoSeleccionado = 5;
            });

    // =====================================
    // MAPA
    // =====================================

    escena = new QGraphicsScene(this);

    ui->graphicsViewMapa->setScene(escena);

    // =====================================
    // COORDENADAS
    // =====================================

    const int CP_X = 40;
    const int CP_Y = 50;

    const int CC_X = 120;
    const int CC_Y = 50;

    const int VM_X = 250;
    const int VM_Y = 140;

    const int RC_X = 150;
    const int RC_Y = 320;

    const int BV_X = 380;
    const int BV_Y = 140;

    const int SF_X = 520;
    const int SF_Y = 80;

    // =====================================
    // ESTILO
    // =====================================

    QPen borde(Qt::black);
    borde.setWidth(2);

    QBrush relleno(Qt::cyan);

    // =====================================
    // RUTAS
    // =====================================

    ruta_CP_CC =
        escena->addLine(
            CP_X, CP_Y,
            CC_X, CC_Y
            );

    ruta_CC_VM =
        escena->addLine(
            CC_X, CC_Y,
            VM_X, VM_Y
            );

    ruta_CC_BV =
        escena->addLine(
            CC_X, CC_Y,
            BV_X, BV_Y
            );

    ruta_VM_BV =
        escena->addLine(
            VM_X, VM_Y,
            BV_X, BV_Y
            );

    ruta_VM_RC =
        escena->addLine(
            VM_X, VM_Y,
            RC_X, RC_Y
            );

    ruta_RC_BV =
        escena->addLine(
            RC_X, RC_Y,
            BV_X, BV_Y
            );

    ruta_RC_CP =
        escena->addLine(
            RC_X, RC_Y,
            CP_X, CP_Y
            );

    ruta_BV_SF =
        escena->addLine(
            BV_X, BV_Y,
            SF_X, SF_Y
            );


    // =====================================
    // CIUDADES
    // =====================================

    escena->addEllipse(CP_X - 8, CP_Y - 8,
                       16, 16,
                       borde, relleno);

    escena->addText("Villa Carlos Paz")
        ->setPos(CP_X + 10, CP_Y - 20);

    escena->addEllipse(CC_X - 8, CC_Y - 8,
                       16, 16,
                       borde, relleno);

    escena->addText("Cordoba Capital")
        ->setPos(CC_X + 10, CC_Y - 20);

    escena->addEllipse(VM_X - 8, VM_Y - 8,
                       16, 16,
                       borde, relleno);

    escena->addText("Villa Maria")
        ->setPos(VM_X + 10, VM_Y - 20);

    escena->addEllipse(RC_X - 8, RC_Y - 8,
                       16, 16,
                       borde, relleno);

    escena->addText("Rio Cuarto")
        ->setPos(RC_X + 10, RC_Y - 20);

    escena->addEllipse(BV_X - 8, BV_Y - 8,
                       16, 16,
                       borde, relleno);

    escena->addText("Bell Ville")
        ->setPos(BV_X + 10, BV_Y - 20);

    escena->addEllipse(SF_X - 8, SF_Y - 8,
                       16, 16,
                       borde, relleno);

    escena->addText("San Francisco")
        ->setPos(SF_X + 10, SF_Y - 20);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resetearRutas()
{
    QPen negro(Qt::black);
    negro.setWidth(2);

    ruta_CP_CC->setPen(negro);
    ruta_CC_VM->setPen(negro);
    ruta_CC_BV->setPen(negro);
    ruta_VM_BV->setPen(negro);
    ruta_VM_RC->setPen(negro);
    ruta_RC_BV->setPen(negro);
    ruta_RC_CP->setPen(negro);
    ruta_BV_SF->setPen(negro);
}


//PINTAR RUTA OPTIMA
void MainWindow::pintarRutaOptima(const ResultadoRuta& resultado)
{
    resetearRutas();

    QPen verde(Qt::green);
    verde.setWidth(4);

    for(int i = 0; i < resultado.cantidadCiudades - 1; i++)
    {
        int origen = resultado.camino[i];
        int destino = resultado.camino[i + 1];

        if(
            (origen == 0 && destino == 1) ||
            (origen == 1 && destino == 0)
            )
        {
            ruta_CP_CC->setPen(verde);
        }

        else if(
            (origen == 0 && destino == 2) ||
            (origen == 2 && destino == 0)
            )
        {
            ruta_CC_VM->setPen(verde);
        }

        else if(
            (origen == 0 && destino == 4) ||
            (origen == 4 && destino == 0)
            )
        {
            ruta_CC_BV->setPen(verde);
        }

        else if(
            (origen == 2 && destino == 4) ||
            (origen == 4 && destino == 2)
            )
        {
            ruta_VM_BV->setPen(verde);
        }

        else if(
            (origen == 2 && destino == 3) ||
            (origen == 3 && destino == 2)
            )
        {
            ruta_VM_RC->setPen(verde);
        }

        else if(
            (origen == 3 && destino == 4) ||
            (origen == 4 && destino == 3)
            )
        {
            ruta_RC_BV->setPen(verde);
        }

        else if(
            (origen == 3 && destino == 1) ||
            (origen == 1 && destino == 3)
            )
        {
            ruta_RC_CP->setPen(verde);
        }

        else if(
            (origen == 4 && destino == 5) ||
            (origen == 5 && destino == 4)
            )
        {
            ruta_BV_SF->setPen(verde);
        }
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    if(origenSeleccionado == -1 ||
        destinoSeleccionado == -1)
    {
        return;
    }

    ResultadoRuta resultado =
        sistema.calcularRutaOptima(
            origenSeleccionado,
            destinoSeleccionado
            );

    pintarRutaOptima(resultado);


    QString texto;

    texto += "Ruta encontrada:\n\n";

    for(int i = 0;
         i < resultado.cantidadCiudades;
         i++)
    {
        texto += QString::fromStdString(
            sistema.obtenerNombreCiudad(
                resultado.camino[i]
                )
            );

        if(i < resultado.cantidadCiudades - 1)
            texto += "\n↓\n";
    }

    texto += "\n\nDistancia total: ";
    texto += QString::number(
        resultado.distanciaTotal
        );
    texto += " km";


    ui->textEditResultado->setText(texto);


}

