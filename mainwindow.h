#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <fstream>

#include "sistemagestionlogistico.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void resetearRutas();
    void pintarRutaOptima(const ResultadoRuta& resultado);
    void actualizarAlertasReales();

private slots:
    void on_pushButton_5_clicked(); // Calcular ruta
    void on_pushButton_clicked();   // Agregar Ciudad
    void on_pushButton_2_clicked(); // Agregar Ruta
    void on_pushButton_3_clicked(); // Cortar Ruta
    void on_pushButton_4_clicked(); // Mostrar Historial
    //  void on_pushButton_6_clicked();    // Ver mapa de IDs

private:
    Ui::MainWindow *ui;
    QGraphicsScene *escena;

    QGraphicsLineItem* ruta_CP_CC;
    QGraphicsLineItem* ruta_CC_VM;
    QGraphicsLineItem* ruta_CC_BV;
    QGraphicsLineItem* ruta_VM_BV;
    QGraphicsLineItem* ruta_VM_RC;
    QGraphicsLineItem* ruta_RC_BV;
    QGraphicsLineItem* ruta_RC_CP;
    QGraphicsLineItem* ruta_BV_SF;

    SistemaLogistico sistema;

    int origenSeleccionado = -1;
    int destinoSeleccionado = -1;
};

#endif // MAINWINDOW_H