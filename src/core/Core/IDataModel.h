#pragma once

namespace App
{
    /** Interface for the data model of the application. */
    class IDataModel
    {
    public:
        /** Virtual destructor to allow proper cleanup of derived classes. */
        virtual ~IDataModel () = default;

        virtual bool isShowSomePanel () const = 0;
        virtual bool isShowDemoPanel () const = 0;
        virtual bool isShowDebugPanel () const = 0;

        virtual void setShowSomePanel (bool value) = 0;
        virtual void setShowDemoPanel (bool value) = 0;
        virtual void setShowDebugPanel (bool value) = 0;
    };
}
