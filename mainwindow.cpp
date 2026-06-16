#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <fstream>
#include <iostream>

#include <QInputDialog>
#include <QMessageBox>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QPen>
#include <QBrush>
#include <QFont>
#include <QMenu>
#include <QAction>
#include <QPixmap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

   //apariencia oscura dark
    this->setStyleSheet(
        // Fondo de la ventana principal
        "QMainWindow { background-color: #1E1E24; }"

        // Estilo general para todos los botones del panel
        "QPushButton { "
        "   background-color: #2A2D34; "
        "   color: #EAEAEA; "
        "   border: 2px solid #4A4E59; "
        "   border-radius: 6px; "
        "   padding: 6px 12px; "
        "   font-weight: bold; "
        "   font-family: 'Segoe UI'; "
        "   font-size: 11px; "
        "}"
        "QPushButton:hover { background-color: #3F444E; border-color: #00B4D8; }"
        "QPushButton:pressed { background-color: #0077B6; }"

        // Estilo para el botón principal "Calcular Ruta"
        "QPushButton#pushButton_5 { "
        "   background-color: #0096C7; "
        "   color: #FFFFFF; "
        "   border: 1px solid #03045E; "
        "   font-size: 12px; "
        "}"
        "QPushButton#pushButton_5:hover { background-color: #00B4D8; }"

        // Cuadro de texto resultados
        "QTextEdit { "
        "   background-color: #121214; "
        "   color: #00F5D4; "
        "   border: 2px solid #2A2D34; "
        "   border-radius: 8px; "
        "   font-family: 'Consolas', 'Courier New'; "
        "   font-size: 11px; "
        "   padding: 8px; "
        "}"

        // Estilo de etiquetas de texto flotantes
        "QLabel { "
        "   color: #EAEAEA; "
        "   font-family: 'Segoe UI'; "
        "   font-size: 11px; "
        "   font-weight: bold; "
        "}"
    );

    // evitar cálculos con datos basura
    origenSeleccionado = -1;
    destinoSeleccionado = -1;

    sistema.inicializar();


    // MENU CIUDAD ORIGEN

    QMenu *ciudadOrigen = new QMenu(this);

    // Carga dinámica basada en el estado actual del backend
    int totalCiudadesInicial = sistema.getCantidadCiudades();
    if (totalCiudadesInicial > 0) {
        for (int i = 0; i < totalCiudadesInicial; i++) {
            ciudadOrigen->addAction(QString::fromStdString(sistema.obtenerNombreCiudad(i)));
        }
    } else {
        ciudadOrigen->addAction("Cordoba Capital");
        ciudadOrigen->addAction("Villa Carlos Paz");
        ciudadOrigen->addAction("Villa Maria");
        ciudadOrigen->addAction("Rio Cuarto");
        ciudadOrigen->addAction("Bell Ville");
        ciudadOrigen->addAction("San Francisco");
    }

    ui->ButtonCiudadOrigen->setMenu(ciudadOrigen);
    ui->ButtonCiudadOrigen->setPopupMode(QToolButton::InstantPopup);

    connect(ciudadOrigen, &QMenu::triggered, this, [=](QAction *accion) {
        ui->ButtonCiudadOrigen->setText(accion->text());
        for (int i = 0; i < sistema.getCantidadCiudades(); i++) {
            if (accion->text() == QString::fromStdString(sistema.obtenerNombreCiudad(i))) {
                origenSeleccionado = i;
                break;
            }
        }
    });

    // MENU CIUDAD DESTINO

    QMenu *ciudadDestino = new QMenu(this);
    if (totalCiudadesInicial > 0) {
        for (int i = 0; i < totalCiudadesInicial; i++) {
            ciudadDestino->addAction(QString::fromStdString(sistema.obtenerNombreCiudad(i)));
        }
    } else {
        ciudadDestino->addAction("Cordoba Capital");
        ciudadDestino->addAction("Villa Carlos Paz");
        ciudadDestino->addAction("Villa Maria");
        ciudadDestino->addAction("Rio Cuarto");
        ciudadDestino->addAction("Bell Ville");
        ciudadDestino->addAction("San Francisco");
    }

    ui->ButtonCiudadDestino->setMenu(ciudadDestino);
    ui->ButtonCiudadDestino->setPopupMode(QToolButton::InstantPopup);

    connect(ciudadDestino, &QMenu::triggered, this, [=](QAction *accion) {
        ui->ButtonCiudadDestino->setText(accion->text());
        for (int i = 0; i < sistema.getCantidadCiudades(); i++) {
            if (accion->text() == QString::fromStdString(sistema.obtenerNombreCiudad(i))) {
                destinoSeleccionado = i;
                break;
            }
        }
    });


    escena = new QGraphicsScene(this);
    ui->graphicsViewMapa->setScene(escena);

    escena->setSceneRect(0, 0, 491, 341);
    ui->graphicsViewMapa->setAlignment(Qt::AlignLeft | Qt::AlignTop);

    ui->graphicsViewMapa->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    ui->graphicsViewMapa->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    // Intentamos renderizar a la imagen real, realmente lo intentamossssss
    QPixmap mapaFondo("mapacordoba.jfif");
    if (!mapaFondo.isNull()) {
        escena->setBackgroundBrush(mapaFondo.scaled(491, 341, Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    } else {
        escena->setBackgroundBrush(QBrush(QColor(0xEAEAEA))); // Fondo de auxilio gris claro (CORREGIDO CLAZY)
    }

    // COORDENADAS AJUSTADAS AL MAPA REAL

    const int CC_X = 225; const int CC_Y = 145; // Córdoba Capital
    const int CP_X = 195; const int CP_Y = 147; // Villa Carlos Paz
    const int VM_X = 310; const int VM_Y = 210; // Villa María
    const int BV_X = 365; const int BV_Y = 215; // Bell Ville
    const int RC_X = 180; const int RC_Y = 245; // Río Cuarto
    const int SF_X = 375; const int SF_Y = 135; // San Francisco

    // NODOS Y ARISTAS

    QPen borde(QColor(0x1A1D24)); // CORREGIDO CLAZY
    borde.setWidth(1);
    QPen penRutas(QColor(0x4A5568)); // Rutas en gris (CORREGIDO CLAZY)
    penRutas.setWidth(3);
    QBrush relleno(QColor(0xD90429)); // Nodos en rojo (CORREGIDO CLAZY)

    // Trazado de lineas logisticas

    ruta_CP_CC = escena->addLine(CP_X, CP_Y, CC_X, CC_Y, penRutas);
    ruta_CC_VM = escena->addLine(CC_X, CC_Y, VM_X, VM_Y, penRutas);
    ruta_CC_BV = escena->addLine(CC_X, CC_Y, BV_X, BV_Y, penRutas);
    ruta_VM_BV = escena->addLine(VM_X, VM_Y, BV_X, BV_Y, penRutas);
    ruta_VM_RC = escena->addLine(VM_X, VM_Y, RC_X, RC_Y, penRutas);
    ruta_RC_BV = escena->addLine(RC_X, RC_Y, BV_X, BV_Y, penRutas);
    ruta_RC_CP = escena->addLine(RC_X, RC_Y, CP_X, CP_Y, penRutas);
    ruta_BV_SF = escena->addLine(BV_X, BV_Y, SF_X, SF_Y, penRutas);

    QFont fontCiudades("Segoe UI", 8, QFont::Bold);

    // Pinta los nodos y pone las ciudades
    escena->addEllipse(CP_X - 6, CP_Y - 6, 12, 12, borde, relleno);
    QGraphicsTextItem* txtCP = escena->addText("Carlos Paz");
    txtCP->setDefaultTextColor(QColor(0x111111));
    txtCP->setFont(fontCiudades);
    txtCP->setPos(CP_X - 65, CP_Y - 20);

    escena->addEllipse(CC_X - 6, CC_Y - 6, 12, 12, borde, relleno);
    QGraphicsTextItem* txtCC = escena->addText("Cordoba Capital");
    txtCC->setDefaultTextColor(QColor(0x111111));
    txtCC->setFont(fontCiudades);
    txtCC->setPos(CC_X - 45, CC_Y - 20);

    escena->addEllipse(VM_X - 6, VM_Y - 6, 12, 12, borde, relleno);
    QGraphicsTextItem* txtVM = escena->addText("Villa Maria");
    txtVM->setDefaultTextColor(QColor(0x111111));
    txtVM->setFont(fontCiudades);
    txtVM->setPos(VM_X + 10, VM_Y - 10);

    escena->addEllipse(RC_X - 6, RC_Y - 6, 12, 12, borde, relleno);
    QGraphicsTextItem* txtRC = escena->addText("Rio Cuarto");
    txtRC->setDefaultTextColor(QColor(0x111111));
    txtRC->setFont(fontCiudades);
    txtRC->setPos(RC_X - 65, RC_Y - 5);

    escena->addEllipse(BV_X - 6, BV_Y - 6, 12, 12, borde, relleno);
    QGraphicsTextItem* txtBV = escena->addText("Bell Ville");
    txtBV->setDefaultTextColor(QColor(0x111111));
    txtBV->setFont(fontCiudades);
    txtBV->setPos(BV_X + 10, BV_Y - 10);

    escena->addEllipse(SF_X - 6, SF_Y - 6, 12, 12, borde, relleno);
    QGraphicsTextItem* txtSF = escena->addText("San Francisco");
    txtSF->setDefaultTextColor(QColor(0x111111));
    txtSF->setFont(fontCiudades);
    txtSF->setPos(SF_X - 35, SF_Y - 20);

    actualizarAlertasReales();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resetearRutas()
{
    // Gris para normal, Rojo para corte
    QPen normal(QColor(0x4A5568)); // Gris original
    normal.setWidth(3);

    QPen cortada(QColor(0xD90429)); // Rojo
    cortada.setWidth(4); // más gruesa para que resalte el bloqueo

    //la referencia segura del grafo
    auto& miGrafo = sistema.getGrafo();

    // Validamos cada tramo individualmente consultando el backend (peso > 9999)

    // Carlos Paz ➔ Córdoba (1 a 0)
    if (miGrafo.obtenerPeso(1, 0) > 9999.0f) ruta_CP_CC->setPen(cortada);
    else ruta_CP_CC->setPen(normal);

    // Córdoba ➔ Villa María (0 a 2)
    if (miGrafo.obtenerPeso(0, 2) > 9999.0f) ruta_CC_VM->setPen(cortada);
    else ruta_CC_VM->setPen(normal);

    // Córdoba ➔ Bell Ville (0 a 3)
    if (miGrafo.obtenerPeso(0, 3) > 9999.0f) ruta_CC_BV->setPen(cortada);
    else ruta_CC_BV->setPen(normal);

    // Villa María ➔ Bell Ville (2 a 3)
    if (miGrafo.obtenerPeso(2, 3) > 9999.0f) ruta_VM_BV->setPen(cortada);
    else ruta_VM_BV->setPen(normal);

    // Villa María ➔ Río Cuarto (2 a 4)
    if (miGrafo.obtenerPeso(2, 4) > 9999.0f) ruta_VM_RC->setPen(cortada);
    else ruta_VM_RC->setPen(normal);

    // Río Cuarto ➔ Bell Ville (4 a 3)
    if (miGrafo.obtenerPeso(4, 3) > 9999.0f) ruta_RC_BV->setPen(cortada);
    else ruta_RC_BV->setPen(normal);

    // Río Cuarto ➔ Carlos Paz (4 a 1)
    if (miGrafo.obtenerPeso(4, 1) > 9999.0f) ruta_RC_CP->setPen(cortada);
    else ruta_RC_CP->setPen(normal);

    // Bell Ville ➔ San Francisco (3 a 5)
    if (miGrafo.obtenerPeso(3, 5) > 9999.0f) ruta_BV_SF->setPen(cortada);
    else ruta_BV_SF->setPen(normal);
}

void MainWindow::pintarRutaOptima(const ResultadoRuta& resultado)
{
    resetearRutas();

    QPen azulLogistico(QColor(0x0077B6)); // Lz línea azul para marcar el trayecto óptimo
    azulLogistico.setWidth(5);

    for(int i = 0; i < resultado.cantidadCiudades - 1; i++)
    {
        int origen = resultado.camino[i];
        int destino = resultado.camino[i + 1];

        if((origen == 1 && destino == 0) || (origen == 0 && destino == 1))      ruta_CP_CC->setPen(azulLogistico);
        else if((origen == 0 && destino == 2) || (origen == 2 && destino == 0)) ruta_CC_VM->setPen(azulLogistico);
        else if((origen == 0 && destino == 3) || (origen == 3 && destino == 0)) ruta_CC_BV->setPen(azulLogistico);
        else if((origen == 2 && destino == 3) || (origen == 3 && destino == 2)) ruta_VM_BV->setPen(azulLogistico);
        else if((origen == 2 && destino == 4) || (origen == 4 && destino == 2)) ruta_VM_RC->setPen(azulLogistico);
        else if((origen == 4 && destino == 3) || (origen == 3 && destino == 4)) ruta_RC_BV->setPen(azulLogistico);
        else if((origen == 4 && destino == 1) || (origen == 1 && destino == 4)) ruta_RC_CP->setPen(azulLogistico);
        else if((origen == 3 && destino == 5) || (origen == 5 && destino == 3)) ruta_BV_SF->setPen(azulLogistico);
    }
}

void MainWindow::on_pushButton_5_clicked()
{
    if(origenSeleccionado == -1 || destinoSeleccionado == -1) {
        ui->textEditResultado->setText("Por favor, seleccione una ciudad de origen y destino.");
        return;
    }

    // Calculamos la ruta
    ResultadoRuta resultado = sistema.calcularRutaOptima(origenSeleccionado, destinoSeleccionado);

    // CONTROL DE SEGURIDAD
    if (resultado.cantidadCiudades <= 1 || resultado.distanciaTotal <= 0) {
        ui->textEditResultado->setText("Error: El sistema no encontró una ruta válida entre estas ciudades.\n"
                                       "Verificá que el grafo esté bien cargado en el backend con los IDs del 0 al 5.");
        resetearRutas();
        return;
    }

    // Si llegó acá, la ruta es válida
    pintarRutaOptima(resultado);

    QString texto = "Ruta encontrada:\n\n";
    for(int i = 0; i < resultado.cantidadCiudades; i++) {
        texto += QString::fromStdString(sistema.obtenerNombreCiudad(resultado.camino[i]));
        if(i < resultado.cantidadCiudades - 1) {
            texto += "\n  ↓\n";
        }
    }

    texto += "\n\nDistancia total: " + QString::number(resultado.distanciaTotal) + " km";
    ui->textEditResultado->setText(texto);

    actualizarAlertasReales();
}

// BOTÓN: AGREGAR CIUDAD

void MainWindow::on_pushButton_clicked()
{
    bool ok;
    // Pedimos el nombre de la nueva ciudad
    QString nombre = QInputDialog::getText(this, "Agregar Ciudad",
                                           "Nombre de la ciudad:", QLineEdit::Normal, "", &ok);
    if (!ok || nombre.isEmpty()) return;

    // Pedimos la coordenada X
    int x = QInputDialog::getInt(this, "Coordenadas",
                                 "Posición X (0 - 491):", 200, 0, 491, 1, &ok);
    if (!ok) return;

    // Pedimos la coordenada y
    int y = QInputDialog::getInt(this, "Coordenadas",
                                 "Posición Y (0 - 341):", 200, 0, 341, 1, &ok);
    if (!ok) return;

    sistema.agregarCiudad(nombre.toStdString(), static_cast<float>(x), static_cast<float>(y));
    sistema.guardarDatos(); // Guarda en el ciudades.dat automáticamente

    // Dibuja el nuevo nodo en el mapa al instante
    QPen borde(QColor(0x1A1D24));
    borde.setWidth(1);
    QBrush relleno(QColor(0xD90429)); // Rojo

    escena->addEllipse(x - 6, y - 6, 12, 12, borde, relleno);
    QGraphicsTextItem* txt = escena->addText(nombre);
    txt->setDefaultTextColor(QColor(0x111111));
    QFont fontCiudades("Segoe UI", 8, QFont::Bold);
    txt->setFont(fontCiudades);
    txt->setPos(x + 10, y - 10);

    // Actualiza los menús desplegables para que aparezca la nueva ciudad
    ui->ButtonCiudadOrigen->menu()->addAction(nombre);
    ui->ButtonCiudadDestino->menu()->addAction(nombre);

    QMessageBox::information(this, "Éxito", "Ciudad '" + nombre + "' agregada y guardada correctamente.");
}

// BOTÓN: AGREGAR RUTA (Conectar ciudades)

void MainWindow::on_pushButton_2_clicked()
{
    // Primero LE MOSTRAMOS PRIMERO LA LISTA DE IDs PARA QUE SEPA QUÉ PONER
    QString guiaIds = "=== GUÍA DE IDs DE CIUDADES ===\n\n";
    int total = sistema.getCantidadCiudades();
    for (int i = 0; i < total; i++) {
        guiaIds += QString::number(i) + " ➔ " + QString::fromStdString(sistema.obtenerNombreCiudad(i)) + "\n";
    }

    QMessageBox::information(this, "¡Atención!", "Antes de continuar, memorice o anote los IDs de las ciudades que quiere conectar:\n\n" + guiaIds);

    // AHORA SÍ, PEDIMOS LOS DATOS
    bool ok;
    int origen = QInputDialog::getInt(this, "Nueva Ruta",
                                      "ID Ciudad Origen:", 0, 0, total - 1, 1, &ok);
    if (!ok) return;

    int destino = QInputDialog::getInt(this, "Nueva Ruta",
                                       "ID Ciudad Destino:", 1, 0, total - 1, 1, &ok);
    if (!ok) return;

    float km = QInputDialog::getDouble(this, "Nueva Ruta",
                                       "Distancia en Kilómetros (km):", 100.0, 1.0, 2000.0, 1, &ok);
    if (!ok) return;

    // Registra en el backend
    sistema.agregarRuta(origen, destino, km);
    QMessageBox::information(this, "Éxito", "Ruta conectada con éxito en el backend.");
}

// BOTÓN: CORTAR RUTA (Bloqueo de caminos)

void MainWindow::on_pushButton_3_clicked()
{
    // PRIMERO LA LISTA DE IDs PARA QUE SEPA QUÉ CORTAR
    QString guiaIds = "=== GUÍA DE IDs DE CIUDADES ===\n\n";
    int total = sistema.getCantidadCiudades();
    for (int i = 0; i < total; i++) {
        guiaIds += QString::number(i) + " ➔ " + QString::fromStdString(sistema.obtenerNombreCiudad(i)) + "\n";
    }

    QMessageBox::warning(this, "¡Atención!", "Antes de continuar, verifique los IDs de las ciudades cuya ruta quiere bloquear:\n\n" + guiaIds);

    // AHORA SÍ, PEDIMOS LOS DATOS PARA CORTAR
    bool ok;
    int origen = QInputDialog::getInt(this, "Cortar Ruta",
                                      "ID Ciudad Origen:", 0, 0, total - 1, 1, &ok);
    if (!ok) return;

    int destino = QInputDialog::getInt(this, "Cortar Ruta",
                                       "ID Ciudad Destino:", 1, 0, total - 1, 1, &ok);
    if (!ok) return;

    // Desconectamos en el backend
    sistema.cortarRuta(origen, destino);

    // Reseteamos visualmente el mapa por seguridad
    resetearRutas();

    QMessageBox::warning(this, "Ruta Cortada", "El camino entre los IDs seleccionados se anuló con éxito.");

    actualizarAlertasReales();
}
// BOTÓN: MOSTRAR HISTORIAL

void MainWindow::on_pushButton_4_clicked()
{
    // Lee el archivo txt
    std::ifstream archivo("historial.txt");
    if (!archivo.is_open()) {
        QMessageBox::information(this, "Historial de Búsquedas", "El historial está vacío actualmente.");
        return;
    }

    QString contenidoHistorial = "** ÚLTIMOS VIAJES CONSULTADOS **\n\n";
    std::string linea;

    // formato origen|destino|distancia
    std::string orig, dest;
    float dist;

    while (std::getline(archivo, orig, '|')) {
        if (std::getline(archivo, dest, '|')) {
            archivo >> dist;
            archivo.ignore(); // Limpiar el salto de línea

            contenidoHistorial += "Origen: " + QString::fromStdString(orig) + "\n";
            contenidoHistorial += "Destino: " + QString::fromStdString(dest) + "\n";
            contenidoHistorial += "Distancia: " + QString::number(dist) + " km\n";
            contenidoHistorial += "--------------------------------------\n";
        }
    }
    archivo.close();

    // Lo muesta en el cuadro de texto
    ui->textEditResultado->setText(contenidoHistorial);
}

void MainWindow::actualizarAlertasReales() {
    ui->textBrowserAlertas->clear();

    QString contenido = "<b>⚠️ ESTADO DE LA RED VIAL EN VIVO:</b><br><br>";
    bool hayCortes = false;

    // Lista fija de las conexiones originales del cargarMapaInicial()
    // Para verificar si fueron cortadas en la matriz de adyacencia
    struct Conexion { int orig; int dest; QString nombreOrig; QString nombreDest; };
    Conexion rutasVerificar[] = {
        {1, 0, "Carlos Paz", "Córdoba"},
        {0, 2, "Córdoba", "Villa María"},
        {0, 3, "Córdoba", "Bell Ville"},
        {2, 3, "Villa María", "Bell Ville"},
        {2, 4, "Villa María", "Río Cuarto"},
        {4, 3, "Río Cuarto", "Bell Ville"},
        {4, 1, "Río Cuarto", "Carlos Paz"},
        {3, 5, "Bell Ville", "San Francisco"}
    };

    // se pide la matriz de adyacencia al grafo para revisar los estados
    // y accede a getGrafo() que devuelve la referencia segura
    auto& miGrafo = sistema.getGrafo();

    for (const auto& ruta : rutasVerificar) {
        //si existe un corte, el peso de la matriz se vuelve muy grande (infinito)
        // entonces verificamos si la distancia actual es considerada inifinita
        if (miGrafo.obtenerPeso(ruta.orig, ruta.dest) > 9999.0f) {
            contenido += "<span style='color: #FF5555;'>• <b>CORTADA:</b></span> Conexión directa entre "
                         + ruta.nombreOrig + " y " + ruta.nombreDest + "<br>";
            hayCortes = true;
        }
    }

    if (!hayCortes) {
        contenido += "<span style='color: #55FF55;'>• <b>SISTEMA NORMAL:</b></span> Todas las rutas principales de la provincia se encuentran habilitadas.";
    } else {
        contenido += "<br><i style='color: #AAAAAA;'>Se recalculará el camino evitando los tramos en rojo.</i>";
    }

    ui->textBrowserAlertas->setHtml(contenido);
}