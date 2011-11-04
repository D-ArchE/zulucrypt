/*
 * 
 *  Copyright (c) 2011
 *  name : mhogo mchungu 
 *  email: mhogomchungu@gmail.com
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "createpartition.h"
#include "ui_createpartition.h"
#include <QMessageBox>
#include <QFileDialog>
#include <iostream>
#include <QAbstractButton>
#include <QProcess>
#include <QThread>

#include "executables.h"

createpartition::createpartition(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::createpartition)
{
	ui->setupUi(this);

	this->setFixedSize(this->size());

	ui->lineEditVolumePath->setEnabled(false);

	ui->lineEditPassphrase1->setFocus();

	connect(ui->pbOpenKeyFile,
		SIGNAL(clicked()),
		this,
		SLOT(pbOpenKeyFile()));
	connect(ui->pbCreate,
		SIGNAL(clicked()),
		this,
		SLOT(pbCreateClicked()));
	connect(ui->pbCancel,
		SIGNAL(clicked()),
		this,
		SLOT(pbCancelClicked()));
	connect(ui->rbPassphrase,
		SIGNAL(clicked()),
		this,
		SLOT(rbPassphraseClicked()));
	connect(ui->rbPassphraseFromFile,
		SIGNAL(clicked()),
		this,
		SLOT(rbPasssphraseFromFileClicked()));
}

void createpartition::ShowPartitionUI(QString volume)
{
	ui->rbPassphrase->setChecked(true);
	ui->lineEditVolumePath->setText(volume);
	ui->rbLuks->setChecked(true);
	ui->rbext4->setChecked(true);	
	ui->labelVolumePath->setText(tr("path to partition"));
	ui->labelPassPhrase->setText(tr("passphrase"));
	this->rbPassphraseClicked() ;
	ui->lineEditPassphrase1->setFocus();
	this->show();
}

void createpartition::pbOpenKeyFile()
{
	QString Z = QFileDialog::getOpenFileName(this,
						 tr("key file path"),
						 QDir::homePath(),
						 0);
	ui->lineEditPassphrase1->setText(Z);
}

void createpartition::pbCancelClicked()
{
	QMessageBox m ;
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.setText(tr("Are you sure you want to cancel this operation?"));
	m.setWindowTitle(tr("WARNING"));
	m.addButton(QMessageBox::Yes);
	m.addButton(QMessageBox::No);
	m.setFont(this->font());

	if( m.exec() == QMessageBox::No )
		return ;

	if( file.isEmpty() == false ){
		QFile::remove(file) ;
		file.clear();
	}
	HideUI() ;
}

void createpartition::HideUI()
{
	this->hide();
}

void createpartition::enableAll()
{
	ui->groupBox->setEnabled(true);
	ui->groupBox_2->setEnabled(true);
	ui->groupBox_3->setEnabled(true);
	ui->labelPassPhrase->setEnabled(true);
	ui->labelVolumePath->setEnabled(true);
	ui->label_3->setEnabled(true);
	ui->lineEditPassphrase1->setEnabled(true);
	ui->lineEditPassPhrase2->setEnabled(true);
	ui->lineEditVolumePath->setEnabled(true);
	ui->pbCancel->setEnabled(true);
	ui->pbCreate->setEnabled(true);
	ui->pbOpenKeyFile->setEnabled(true);
}

void createpartition::disableAll()
{
	ui->groupBox->setEnabled(false);
	ui->groupBox_2->setEnabled(false);
	ui->groupBox_3->setEnabled(false);
	ui->labelPassPhrase->setEnabled(false);
	ui->labelVolumePath->setEnabled(false);
	ui->label_3->setEnabled(false);
	ui->lineEditPassphrase1->setEnabled(false);
	ui->lineEditPassPhrase2->setEnabled(false);
	ui->lineEditVolumePath->setEnabled(false);
	ui->pbCancel->setEnabled(false);
	ui->pbCreate->setEnabled(false);
	ui->pbOpenKeyFile->setEnabled(false);
}

void createpartition::rbPassphraseClicked()
{
	ui->pbOpenKeyFile->setEnabled(false);
	ui->lineEditPassPhrase2->setEnabled(true);
	ui->lineEditPassphrase1->clear();
	ui->lineEditPassPhrase2->clear();
	ui->lineEditPassphrase1->setEchoMode(QLineEdit::Password);
	ui->lineEditPassPhrase2->setEchoMode(QLineEdit::Password);
	ui->labelPassPhrase->setText(tr("passphrase"));
}

void createpartition::rbPasssphraseFromFileClicked()
{
	ui->pbOpenKeyFile->setEnabled(true);
	ui->lineEditPassphrase1->clear();
	ui->lineEditPassPhrase2->clear();
	ui->lineEditPassphrase1->setEchoMode(QLineEdit::Normal);
	ui->lineEditPassPhrase2->setEnabled(false);
	ui->labelPassPhrase->setText(tr("key file"));
}

void createpartition::UIMessage(QString title, QString message)
{
	QMessageBox m ;
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.setText(message);
	m.setWindowTitle(title);
	m.addButton(QMessageBox::Ok);
	m.setFont(this->font());
	m.exec() ;
}

void createpartition::pbCreateClicked()
{
	if( ui->lineEditVolumePath->text().isEmpty() == true )	{

		UIMessage(tr("ERROR!"),tr("volume path field is empty"));
		return ;
	}

	if( ui ->lineEditPassphrase1->text().isEmpty() == true){

		if( ui->rbPassphrase->isChecked() == true)
			UIMessage(tr("ERROR"),tr("passphrases field is empty"));
		else
			UIMessage(tr("ERROR"),tr("key file field is empty"));

		return ;
	}

	if(ui->rbPassphrase->isChecked() == true){

		if( QString::compare(ui->lineEditPassphrase1->text(),
				     ui->lineEditPassPhrase2->text()) != 0 ){
			UIMessage(tr("ERROR"),tr("passphrases do not match"));
			return ;
		}
	}

	QMessageBox m ;
	m.setFont(this->font());
	m.setParent(this);
	m.setWindowFlags(Qt::Window | Qt::Dialog);
	m.setWindowTitle(tr("WARNING"));

	m.addButton(QMessageBox::Yes);
	m.addButton(QMessageBox::No);
	m.setDefaultButton(QMessageBox::No);

	QString wr = tr("all contents of \"") ;
	wr = wr + ui->lineEditVolumePath->text() + tr("\" will be deleted!.");
	wr = wr + tr("\nAre you sure you want to proceed?") ;
	m.setText(wr);

	if ( m.exec() != QMessageBox::Yes )
		return ;

	cvt = new createvolumeThread(ui,&status) ;

	connect(cvt,SIGNAL(finished()),this,SLOT(threadfinished())) ;

	disableAll();

	cvt->start();
}

void createpartition::threadfinished()
{
	delete cvt ;

	switch( status ) {
		case 0 : UIMessage(tr("SUCCESS"),
				   tr("volume created successfully"));
			HideUI();
			break;
		case 1 : UIMessage(tr("ERROR"),
			tr("File path given does not point to a file or partition"));
			break ;			
		case 6 : UIMessage(tr("ERROR"),
				   tr("couldnt get requested memory to open the key file"));
			break ;
		default: UIMessage(tr("ERROR"),
				   (tr("unrecognized error has occured,volume not created")));
	}
	enableAll();
}

void createpartition::ShowFileUI(QString volume)
{
	enableAll();
	ui->lineEditVolumePath->setText(volume);
	ui->rbLuks->setChecked(true);
	ui->rbext4->setChecked(true);
	ui->rbPassphrase->setChecked(true);
	ui->pbOpenKeyFile->setEnabled(false);	
	ui->labelVolumePath->setText(tr("path to file"));
	ui->lineEditVolumePath->setEnabled(false);
	file = ui->lineEditVolumePath->text() ;
	this->rbPassphraseClicked() ;
	ui->lineEditPassphrase1->setFocus();
	this->show();
}

createpartition::~createpartition()
{
    delete ui;
}
