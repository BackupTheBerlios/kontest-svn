#ifndef DXCLUSTERSPOTITEM_H
#define DXCLUSTERSPOTITEM_H
#include <QListWidgetItem>

class dxClusterSpotItem : public QListWidgetItem {
  public:
    dxClusterSpotItem( QListWidget* parent, const QString& spot, const QColor& color );
    ~dxClusterSpotItem();


  protected:
  
  QColor spotColor;

};


#endif
