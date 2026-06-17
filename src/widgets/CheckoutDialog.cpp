#include "widgets/CheckoutDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QGroupBox>

CheckoutDialog::CheckoutDialog(const Order &order, QWidget *parent)
    : QDialog(parent), m_order(order) {
    setupUI();
    updateReceipt();
}

void CheckoutDialog::setupUI() {
    setWindowTitle("Checkout");
    setMinimumWidth(400);

    QVBoxLayout *layout = new QVBoxLayout(this);

    // Order Summary
    QGroupBox *summaryGroup = new QGroupBox("Order Summary", this);
    QVBoxLayout *summaryLayout = new QVBoxLayout(summaryGroup);

    m_subtotalLabel = new QLabel(this);
    summaryLayout->addWidget(m_subtotalLabel);

    m_serviceChargeLabel = new QLabel("Service Charge (10%): $0.00", this);
    summaryLayout->addWidget(m_serviceChargeLabel);

    m_totalLabel = new QLabel(this);
    QFont totalFont = m_totalLabel->font();
    totalFont.setPointSize(14);
    totalFont.setBold(true);
    m_totalLabel->setFont(totalFont);
    summaryLayout->addWidget(m_totalLabel);

    layout->addWidget(summaryGroup);

    // Payment Method
    QGroupBox *paymentGroup = new QGroupBox("Payment Method", this);
    QVBoxLayout *paymentLayout = new QVBoxLayout(paymentGroup);

    m_cashRadio = new QRadioButton("Cash", this);
    m_cashRadio->setChecked(true);
    paymentLayout->addWidget(m_cashRadio);

    m_cardRadio = new QRadioButton("Credit Card", this);
    paymentLayout->addWidget(m_cardRadio);

    m_memberRadio = new QRadioButton("Member Balance", this);
    paymentLayout->addWidget(m_memberRadio);

    m_paymentGroup = new QButtonGroup(this);
    m_paymentGroup->addButton(m_cashRadio);
    m_paymentGroup->addButton(m_cardRadio);
    m_paymentGroup->addButton(m_memberRadio);

    connect(m_cashRadio, &QRadioButton::toggled, this, [this]() {
        if (m_cashRadio->isChecked()) {
            m_selectedMethod = PaymentMethod::Cash;
            paymentMethodChanged();
        }
    });
    connect(m_cardRadio, &QRadioButton::toggled, this, [this]() {
        if (m_cardRadio->isChecked()) {
            m_selectedMethod = PaymentMethod::CreditCard;
            paymentMethodChanged();
        }
    });
    connect(m_memberRadio, &QRadioButton::toggled, this, [this]() {
        if (m_memberRadio->isChecked()) {
            m_selectedMethod = PaymentMethod::MemberBalance;
            paymentMethodChanged();
        }
    });

    layout->addWidget(paymentGroup);

    // Cash Input
    QHBoxLayout *cashLayout = new QHBoxLayout();
    cashLayout->addWidget(new QLabel("Cash Received: $", this));
    m_cashInput = new QLineEdit(this);
    m_cashInput->setPlaceholderText("0.00");
    cashLayout->addWidget(m_cashInput);
    layout->addLayout(cashLayout);

    connect(m_cashInput, &QLineEdit::textChanged, this, &CheckoutDialog::calculateChange);

    // Change Display
    m_changeLabel = new QLabel("Change: $0.00", this);
    m_changeLabel->setStyleSheet("color: green; font-weight: bold;");
    layout->addWidget(m_changeLabel);

    // Buttons
    m_buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
    layout->addWidget(m_buttonBox);
}

void CheckoutDialog::updateReceipt() {
    double subtotal = m_order.subtotal / 100.0;
    double serviceCharge = (m_order.subtotal * 0.1) / 100.0; // 10% service charge
    double total = (m_order.total + m_order.subtotal * 0.1) / 100.0;

    m_subtotalLabel->setText(QString("Subtotal: $%1").arg(subtotal, 0, 'f', 2));
    m_serviceChargeLabel->setText(QString("Service Charge (10%): $%1").arg(serviceCharge, 0, 'f', 2));
    m_totalLabel->setText(QString("Total: $%1").arg(total, 0, 'f', 2));
}

void CheckoutDialog::calculateChange() {
    if (m_selectedMethod != PaymentMethod::Cash) {
        m_changeLabel->setText("Change: $0.00");
        return;
    }

    double total = (m_order.total + m_order.subtotal * 0.1) / 100.0;
    double cashReceived = m_cashInput->text().toDouble();
    double change = cashReceived - total;

    if (change >= 0) {
        m_changeLabel->setText(QString("Change: $%1").arg(change, 0, 'f', 2));
        m_changeLabel->setStyleSheet("color: green; font-weight: bold;");
    } else {
        m_changeLabel->setText(QString("Insufficient payment"));
        m_changeLabel->setStyleSheet("color: red; font-weight: bold;");
    }
}

void CheckoutDialog::paymentMethodChanged() {
    m_cashInput->setEnabled(m_selectedMethod == PaymentMethod::Cash);
    m_changeLabel->setVisible(m_selectedMethod == PaymentMethod::Cash);
    if (m_selectedMethod != PaymentMethod::Cash) {
        m_changeLabel->setText("Change: $0.00");
    }
}

int CheckoutDialog::getPaymentAmount() const {
    return (m_order.total + m_order.subtotal * 0.1); // total + 10% service charge
}

PaymentMethod CheckoutDialog::getPaymentMethod() const {
    return m_selectedMethod;
}