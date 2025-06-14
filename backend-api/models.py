from sqlalchemy import Column, Integer, String, DateTime
from sqlalchemy.orm import Mapped
from sqlalchemy.orm import mapped_column
from datetime import datetime
from database import Base


class Order(Base):
    __tablename__ = "Order"

    # Mapped[] variables will be mapped by the ORM to the DB
    # ClassVar[] won't be mapped by the ORM to the DB
    # see https://docs.sqlalchemy.org/en/20/tutorial/metadata.html#tutorial-orm-table-metadata
    # Declaring Mapped Classes
    ID: Mapped[int] = mapped_column(Integer, primary_key=True, index=True)
    Quantity: Mapped[str] = mapped_column(String)
    Price: Mapped[str] = mapped_column(String)

    def __repr__(self):
        return f"Order : (ID={self.ID}, Quantity={self.Quantity}, Price={self.Price})"


class AlgoScript(Base):
    __tablename__ = "AlgoScript"

    #ID: Mapped[int] = mapped_column(Integer, primary_key=True, index=True)
    #CreationTime: Mapped[datetime] = mapped_column(DateTime)
    Title: Mapped[str] = mapped_column(String, primary_key=True, nullable = False)
    User: Mapped[str] = mapped_column(String, primary_key=True, nullable = False)
    Summary: Mapped[str] = mapped_column(String)
    Content: Mapped[str] = mapped_column(String)
