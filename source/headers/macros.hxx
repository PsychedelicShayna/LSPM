#ifndef MACROS_HXX
#define MACROS_HXX

/* This is a macro for creating a static QMessageBox on the heap, that will automatically be deleted once
 * the macro is reached again, resulting in an auto-deleting message box. This is done because there is no
 * good way to know when a Qt object allocated on the heap can be deleted, without conflicting with an open
 * Ui. Rather than an event system, or a thread based system, this is the simplest solution. */

#define mbalert(msg, parent) \
    static QMessageBox* error_message_box = nullptr; \
    if(error_message_box != nullptr) delete error_message_box; \
    error_message_box = new QMessageBox(parent); \
    error_message_box->setText(msg); \
    error_message_box->show();

#endif // MACROS_HXX
