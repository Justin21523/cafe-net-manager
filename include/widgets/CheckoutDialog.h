#pragma once

#include <QDialog>
#include "models/Order.h"
#include "models/Payment.h"

class QLabel;
class QRadioButton;
class QButtonGroup;
class QLineEdit;
class QDialogButtonBox;

class CheckoutDialog : public QDialog {
    Q_OBJECT

public:
    explicit CheckoutDialog(const Order &order, QWidget *parent = nullptr);

    int getPaymentAmount() const;
    PaymentMethod getPaymentMethod() const;

private slots:
    void calculateChange();
    void paymentMethodChanged();

private:
    void setupUI();
    void updateReceipt();

    Order m_order;
    QLabel *m_subtotalLabel;
    QLabel *m_serviceChargeLabel;
    QLabel *m_totalLabel;
    
    QRadioButton *m_cashRadio;
    QRadioButton *m_cardRadio;
    QRadioButton *m_memberRadio;
    QButtonGroup *m_paymentGroup;
    
    QLineEdit *m_cashInput;
    QLabel *m_changeLabel;
    
    QDialogButtonBox *m_buttonBox;
    
    PaymentMethod m_selectedMethod = PaymentMethod::Cash;
};