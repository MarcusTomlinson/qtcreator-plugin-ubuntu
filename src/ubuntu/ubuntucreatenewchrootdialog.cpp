#include "ubuntucreatenewchrootdialog.h"
#include "ui_ubuntucreatenewchrootdialog.h"

#include "ubuntuconstants.h"
namespace Ubuntu {

namespace Constants {
    const char* UBUNTU_CLICK_SUPPORTED_ARCHS[]      = {"armhf","i386","amd64","\0"};

    //lists all currently supported targets by the plugin
    const char* UBUNTU_CLICK_SUPPORTED_TARGETS[][3]   = {
        //Series      Framework         Displayname
        {"trusty","ubuntu-sdk-13.10","Trusty (14.04)"},
        {"saucy" ,"ubuntu-sdk-13.10","Saucy (13.10)"},
        {"\0","\0","\0"}
    };
}

namespace Internal {

UbuntuCreateNewChrootDialog::UbuntuCreateNewChrootDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UbuntuCreateNewChrootDialog)
{
    ui->setupUi(this);

    //add supported targets
    for(int i = 0; Constants::UBUNTU_CLICK_SUPPORTED_TARGETS[i][0][0] != '\0'; i++){
        ui->comboBoxSeries->addItem(QLatin1String(Constants::UBUNTU_CLICK_SUPPORTED_TARGETS[i][2]),i);
    }

    //add supported architectures
    for(int i = 0; Constants::UBUNTU_CLICK_SUPPORTED_ARCHS[i][0] != '\0' ;i++) {
        ui->comboBoxArch->addItem(QLatin1String(Constants::UBUNTU_CLICK_SUPPORTED_ARCHS[i]));
    }
}

UbuntuCreateNewChrootDialog::~UbuntuCreateNewChrootDialog()
{
    delete ui;
}

/**
 * @brief UbuntuCreateNewChrootDialog::getNewChrootParams
 * Opens a dialog that lets the user select a new chroot, returns false
 * if the user pressed cancel
 */
bool UbuntuCreateNewChrootDialog::getNewChrootTarget(UbuntuClickTool::Target *target)
{
    UbuntuCreateNewChrootDialog dlg;
    if( dlg.exec() == QDialog::Accepted) {
        bool ok = false;

        int idx = dlg.ui->comboBoxSeries->itemData(dlg.ui->comboBoxSeries->currentIndex()).toInt(&ok);
        if(!ok)
            return false;

        target->architecture = dlg.ui->comboBoxArch->currentText();
        target->series       = QString::fromLatin1(Constants::UBUNTU_CLICK_SUPPORTED_TARGETS[idx][0]);
        target->framework    = QString::fromLatin1(Constants::UBUNTU_CLICK_SUPPORTED_TARGETS[idx][1]);
    }
    return false;
}

} // namespace Internal
} // namespace Ubuntu
